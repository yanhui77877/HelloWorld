#version 330 core

in vec2 uv;
uniform sampler2D screenTexture;
uniform int effectMode;
uniform float exposure;
out vec4 color;

vec3
linearToSRGB(vec3 linearColor)
{
    vec3 low = 12.92*linearColor;
    vec3 high = 1.055*pow(max(linearColor, vec3(0.0)), vec3(1.0/2.4)) - 0.055;
    return mix(high, low, lessThanEqual(linearColor, vec3(0.0031308)));
}

void
main()
{
    vec3 hdrColor = texture(screenTexture, uv).rgb;

vec3 exposedColor = hdrColor * exposure;

vec3 mapped =
    exposedColor / (exposedColor + vec3(1.0));

    color = vec4(linearToSRGB(mapped), 1.0);
}
// void
// main()
// {
//     vec4 sceneColor = texture(screenTexture, uv);

//     if (effectMode == 0)
//     {
//         color = sceneColor;
//         return;
//     }

//     if (effectMode == 1)
//     {
//         color = vec4(vec3(1.0) - sceneColor.rgb, sceneColor.a);
//         return;
//     }

//     if (effectMode == 2)
//     {
//         float gray = dot(sceneColor.rgb, vec3(0.2126, 0.7152, 0.0722));
//         color = vec4(vec3(gray), sceneColor.a);
//         return;
//     }

//     vec2 texelSize = 1.0/vec2(textureSize(screenTexture, 0));
//     float gaussianWeights[9] = float[]
//     (
//         1.0, 2.0, 1.0,
//         2.0, 4.0, 2.0,
//         1.0, 2.0, 1.0
//     );
//     float weights[9] = float[]
//     (
//          0.0, -1.0,  0.0,
//         -1.0,  5.0, -1.0,
//          0.0, -1.0,  0.0
//     );

//     vec3 sum = vec3(0.0);

//     for (int y = -1; y <= 1; ++y)
//         for (int x = -1; x <= 1; ++x)
//         {
//             int index = (y + 1)*3 + (x + 1);
//             vec2 offset = vec2(x, y)*texelSize;
//             vec3 sampleColor = texture(screenTexture, uv + offset).rgb;
//             float weight = effectMode == 3 ? gaussianWeights[index] : weights[index];
//             sum += sampleColor*weight;
//         }

//     if (effectMode == 3)
//         sum /= 16.0;

//     color = vec4(sum, sceneColor.a);
// }
