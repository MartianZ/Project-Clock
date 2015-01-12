module NixieTube(clk, data, pwm, sel, out, en, clk_32k, neon_pwm);
input clk;
input data;
input pwm;
input sel;
input en;
input clk_32k;

output neon_pwm;
output [71:0]out;
reg [71:0]data1;
reg [71:0]data2;
reg [1:0]counter;


always@ (posedge clk)
begin
	if (sel)
	begin
		data1 <= data1 << 1;
		data1[0] <= data;
	end else begin
		data2 <= data2 << 1;
		data2[0] <= data;
	end
end

assign out = (en == 1) ? 
			(pwm == 1) ? data1 : data2
			: 72'b0;
			
always@ (posedge clk_32k)
begin
	counter = counter + 1;
end

assign neon_pwm = counter[1];

endmodule
								