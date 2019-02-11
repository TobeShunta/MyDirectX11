///////////////////////////////////////////////////////////////////////
//	基本的な頂点、ピクセルシェーダ
//	作成者：戸部俊太
//	作成日：2019/2/12
///////////////////////////////////////////////////////////////////////
//	グローバル
cbuffer global
{
	matrix g_WVP;	//	ワールドから射影までの変換行列
};

//	バーテックスシェーダ
float4 VS(float4 Pos : POSITION) : SV_POSITION
{
	Pos = mul(Pos, g_WVP);
	return Pos;
}

//	ピクセルシェーダ
float4 PS( float4 Pos : SV_POSITION) : SV_Target
{
	return float4(1,1,1,1);
}