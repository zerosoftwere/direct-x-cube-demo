uniform float4x4 gWVP;

void ColorVS(in float3 iPositionL : POSITION0, in float4 iColor : COLOR0, 
			out float4 dColor : COLOR0, out float4 oPositionH : POSITION0)
{
	oPositionH = mul(float4(iPositionL, 1.0f), gWVP);
	dColor = iColor;
}

float4 ColorPS(in float4 dColor : COLOR0) : COLOR0
{
	return dColor;
}

technique ColorTech
{
	pass P0
	{
		vertexshader = compile vs_2_0 ColorVS();
		pixelshader  = compile ps_2_0 ColorPS();
		
		fillmode = solid;
	}
}