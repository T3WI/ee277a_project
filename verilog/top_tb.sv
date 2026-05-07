`timescale 1ns/1ns
module top_tb;
    logic clk, reset;

    // 100 MHz System Clock (10ns period)
    initial begin 
        clk = 0;
        reset = 0;
        #200;          // Hold reset long enough for Clock Wizard to lock
        reset = 1;
    end
    always #10 clk = ~clk; // #5 gives a 10ns period (100MHz)
    logic m_data;
    logic mic_irq;
    AHBMIC a(
        .HCLK(clk),            // 50 MHz CLK
        .HRESETn(reset),
        .HADDR(),
        .HTRANS(),
        .HWDATA(),
        .HWRITE(),
        .HREADY(),
    
        .HREADYOUT(),
        .HRDATA(),
    
        .HSEL(),            // never need to use HSEL since this module purely drives interrupts
        .mic_irq(mic_irq),

        .m_clk(),
        .m_data(m_data),
        .m_lrsel()
    );

    initial begin
        m_data = 0;
        wait(!reset); // Wait for reset to be DONE
        
        forever begin 
            // Trigger slightly AFTER the edge to avoid simulation races
            @(posedge clk); 
            #1; // Small delay to ensure clean sampling in simulation
            m_data <= $urandom(); 
        end
    end
    initial begin 
        #1000;
        $finish;
    end
endmodule