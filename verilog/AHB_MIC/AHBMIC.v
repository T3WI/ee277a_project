module AHBMIC(
    input wire         HCLK,            // 50 MHz CLK
    input wire         HRESETn,
    input wire  [31:0] HADDR,
    input wire  [1:0]  HTRANS,
    input wire  [31:0] HWDATA,
    input wire         HWRITE,
    input wire         HREADY,
    
    output wire        HREADYOUT,
    output wire [31:0] HRDATA,
    
    input wire         HSEL,            // never need to use HSEL since this module purely drives interrupts
    output wire        mic_irq,

    output             m_clk,
    input              m_data,
    output             m_lrsel
    );
    localparam THRESHOLD = 24'h0F5555;      // 0F5555 is a level where my voice can be detected when really close to the mic
    localparam HOLD_THRESH = 23438;         //  was 23438
    // Make 3 MHz CLK
    reg [2:0] clk_count;
    reg       clk_mic;
    always @(posedge HCLK) begin 
        if(!HRESETn) begin 
            clk_mic <= 0;
            clk_count <= 0;
        end
        else if(clk_count == 7) begin 
            clk_mic <= ~clk_mic;
            clk_count <= 0;
        end
        else begin 
            clk_count <= clk_count + 1;
        end
    end
    assign m_clk = clk_mic;
    assign m_lrsel = 1;

    
    // take in mic data (PDM) and process it to a readable audio signal (PCM)
    wire signed [23:0] cic_dataout;
    wire cic_mvalid, fir_sready;
    cic c(
        .data_out(cic_dataout),
        .m_valid(cic_mvalid),
        .m_ready(fir_sready),
        .data_in(m_data),
        .clk(clk_mic),
        .resetn(HRESETn)
    );
    // Compensation filter
    wire signed [23:0] fir_dataout;
    wire        fir_mvalid;
    fir f(
        .data_out(fir_dataout),
        .data_in(cic_dataout),
        .s_valid(cic_mvalid),
        .s_ready(fir_sready),
        .m_valid(fir_mvalid),
        .m_ready(1'b1),
        .clk(clk_mic),
        .resetn(HRESETn)
    );

    // padding for HRDATA
    assign HRDATA = {{8{fir_dataout[23]}}, fir_dataout};

    // IRQ
    wire [23:0] pcm_mag;
    assign pcm_mag = fir_dataout[23] ? ~fir_dataout : fir_dataout;  // approximate pcm_mag as 1s complement

    // Volume Threshold detector
    reg [15:0] hold_count;
    localparam IDLE = 1'b0;
    localparam HOLD  = 1'b1;
    reg state;
    reg trigger_out;
    always @(posedge clk_mic, negedge HRESETn) begin 
        if(!HRESETn) begin 
            hold_count <= 0;
            trigger_out <= 0;
            state <= IDLE;
        end 
        else if(fir_mvalid) begin // wait for a valid pcm signal
            case(state) 
                IDLE: begin 
                    if(pcm_mag >= THRESHOLD) begin // wait for an audio signal that is loud enough
                        hold_count <= hold_count + 1;
                        state <= HOLD;
                    end
                    else begin 
                        hold_count <= 0;
                        state <= IDLE;
                    end
                    trigger_out <= 0;
                end
                HOLD: begin 
                    if((pcm_mag >= THRESHOLD) && (hold_count < HOLD_THRESH)) begin // if audio signal meets threshold but not yet loud for long enough, increment counter  
                        state <= HOLD;
                        hold_count <= hold_count + 1;
                        trigger_out <= 0;
                    end
                    else if((pcm_mag >= THRESHOLD) && (hold_count >= HOLD_THRESH)) begin // if held for long enough, go back to idle and trigger irq
                        state <= IDLE;
                        hold_count <= 0;
                        trigger_out <= 1;
                    end
                    else if((pcm_mag < THRESHOLD) && (hold_count == 0)) begin  // if audio signal doesn't meet threshold and hasn't been held at all, go back to idle
                        state <= IDLE;
                        hold_count <= 0;
                        trigger_out <= 0;
                    end
                    else if((pcm_mag < THRESHOLD) && (hold_count < HOLD_THRESH)) begin  // if audio signal doesn't meet threshold, but has held in the past, decrease counter
                        state <= HOLD;
                        hold_count <= hold_count - 1;
                        trigger_out <= 0;
                    end
                    else if((pcm_mag < THRESHOLD) && (hold_count >= HOLD_THRESH)) begin  // if audio signal doesn't meet threshold, but has held in the past, decrease counter
                        state <= IDLE;
                        hold_count <= 0;
                        trigger_out <= 1;
                    end
                end
            endcase
        end
    end
    reg last_trigger_out;
    always @(posedge clk_mic, negedge HRESETn) begin 
        if(!HRESETn) begin 
            last_trigger_out <= 0;
        end
        else begin 
            last_trigger_out <= trigger_out;
        end
    end
    
    assign mic_irq = ~last_trigger_out&trigger_out;
    assign HREADYOUT = 1'b1;
endmodule