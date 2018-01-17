#ifndef _SHADER_GLOBALS_
#define _SHADER_GLOBALS_

/////////////
// GLOBALS //
/////////////
//Texture2D shaderTexture;
//SamplerState SampleType;

//////////////
// TYPEDEFS //
//////////////
float4 sphereMask(float2 tex)
{
    float alpha = 0;

    float radius = .4f;
    float2 offsetFromCenter = tex - float2(0.5f, 0.5f);
    float dist = length(offsetFromCenter);
	
    if (dist <= radius)
    {
        alpha = (1 - dist) * (1 - dist);
    }

    return alpha;
}


#endif //_SHADER_GLOBALS_