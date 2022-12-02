#version 330 core
// This file is for basically postprocessing
/********************** Project 6 ***************************/
uniform bool isKernelPostProcessingOn;
uniform bool isPixelPostProcessingOn;
uniform int textureWidth;
uniform int textureHeight;

uniform bool isEdgeFilterOn;

uniform float filter1[9];
uniform float filter2[9];
// Task 16: Create a UV coordinate in variable
in vec2 uvCoord;
// Task 8: Add a sampler2D uniform
// myTexture holds texture data
uniform sampler2D myTexture;
// Task 29: Add a bool on whether or not to filter the texture
uniform bool isPostProcessingOn;
out vec4 fragColor;

// if uv coord is off the range, return zero color
vec4 giveColor(vec2 uvCoord) {
    if (uvCoord.x < 0.f || uvCoord.x > 1.f || uvCoord.y < 0.f || uvCoord.y > 1.f) {
        return vec4(0,0,0,1);
    } else {
        return texture(myTexture, uvCoord);
    }
}

vec4 fragColors[9];
vec2 uvCoords[9];

// for FXAA extra credit
float rgb2luma(vec3 rgb) {
    return sqrt(dot(rgb, vec3(0.299, 0.587, 0.114)));
}

// for FXAA extra credit
float QUALITY(int i) {
    switch(i) {
        case 0:
        return 1.5;

    case 1:
        return 2.0;
    case 2:
        return 2.0;
    case 3:
        return 2.0;
    case 4:
        return 2.0;
    case 5:
        return 4.0;
    case 6:
        return 8.0;
    case 7:
        return 16.0;
    case 8:
        return 32.0;
    case 9:
        return 64.0;
    case 10:
        return 128.0;
    case 11:
        return 256.0;
    }
}

vec4 FXAA() {
    float u = 1.f / textureWidth; // textureWidth is basically width of screen?
    float v = 1.f / textureHeight;

    vec3 colorCenter = texture(myTexture, uvCoord).rgb;

    float lumaCenter = rgb2luma(colorCenter);

    float lumaDown = rgb2luma(textureOffset(myTexture, uvCoord, ivec2(0,-1)).rgb);
    float lumaUp = rgb2luma(textureOffset(myTexture, uvCoord, ivec2(0,1)).rgb);
    float lumaLeft = rgb2luma(textureOffset(myTexture, uvCoord, ivec2(-1,0)).rgb);
    float lumaRight = rgb2luma(textureOffset(myTexture, uvCoord, ivec2(1,0)).rgb);

    float lumaMin = min(lumaCenter, min(min(lumaDown,lumaUp), min(lumaLeft,lumaRight)));
    float lumaMax = max(lumaCenter, max(max(lumaDown,lumaUp), max(lumaLeft,lumaRight)));

    float lumaRange = lumaMax - lumaMin;

    // if not on edge, don't apply FAXX
    float EDGE_THRESHOLD_MIN = 0.0312;
    float EDGE_THRESHOLD_MAX = 0.125;
    if(lumaRange < max(EDGE_THRESHOLD_MIN,lumaMax*EDGE_THRESHOLD_MAX)) {
        fragColor = vec4(colorCenter,1);
    }

    float lumaDownLeft = rgb2luma(textureOffset(myTexture, uvCoord, ivec2(-1,-1)).rgb);
    float lumaUpRight = rgb2luma(textureOffset(myTexture, uvCoord, ivec2(1,1)).rgb);
    float lumaUpLeft = rgb2luma(textureOffset(myTexture, uvCoord, ivec2(-1,1)).rgb);
    float lumaDownRight = rgb2luma(textureOffset(myTexture, uvCoord, ivec2(1,-1)).rgb);

    float lumaDownUp = lumaDown + lumaUp;
    float lumaLeftRight = lumaLeft + lumaRight;

    float lumaLeftCorners = lumaDownLeft + lumaUpLeft;
    float lumaDownCorners = lumaDownLeft + lumaDownRight;
    float lumaRightCorners = lumaDownRight + lumaUpRight;
    float lumaUpCorners = lumaUpRight + lumaUpLeft;

    float edgeHorizontal =  abs(-2.0 * lumaLeft + lumaLeftCorners)  + abs(-2.0 * lumaCenter + lumaDownUp ) * 2.0    + abs(-2.0 * lumaRight + lumaRightCorners);
    float edgeVertical =    abs(-2.0 * lumaUp + lumaUpCorners)      + abs(-2.0 * lumaCenter + lumaLeftRight) * 2.0  + abs(-2.0 * lumaDown + lumaDownCorners);

    // tells if local edge horizontal or vertical
    bool isHorizontal = (edgeHorizontal >= edgeVertical);

    float luma1 = isHorizontal ? lumaDown : lumaLeft;
    float luma2 = isHorizontal ? lumaUp : lumaRight;

    float gradient1 = luma1 - lumaCenter;
    float gradient2 = luma2 - lumaCenter;

    // find steepest gradient
    bool is1Steepest = abs(gradient1) >= abs(gradient2);

    float gradientScaled = 0.25*max(abs(gradient1),abs(gradient2));

    vec2 inverseScreenSize = vec2(u,v);
    float stepLength = isHorizontal ? inverseScreenSize.y : inverseScreenSize.x;

    float lumaLocalAverage = 0.0;

    if(is1Steepest){
        // Switch the direction
        stepLength = - stepLength;
        lumaLocalAverage = 0.5*(luma1 + lumaCenter);
    } else {
        lumaLocalAverage = 0.5*(luma2 + lumaCenter);
    }

    vec2 currentUv = uvCoord;
    if(isHorizontal){
        currentUv.y += stepLength * 0.5;
    } else {
        currentUv.x += stepLength * 0.5;
    }

    vec2 offset = isHorizontal ? vec2(inverseScreenSize.x,0.0) : vec2(0.0,inverseScreenSize.y);
    // Compute UVs to explore on each side of the edge, orthogonally. The QUALITY allows us to step faster.
    vec2 uv1 = currentUv - offset;
    vec2 uv2 = currentUv + offset;

    // Read the lumas at both current extremities of the exploration segment, and compute the delta wrt to the local average luma.
    float lumaEnd1 = rgb2luma(texture(myTexture,uv1).rgb);
    float lumaEnd2 = rgb2luma(texture(myTexture,uv2).rgb);
    lumaEnd1 -= lumaLocalAverage;
    lumaEnd2 -= lumaLocalAverage;

    // If the luma deltas at the current extremities are larger than the local gradient, we have reached the side of the edge.
    bool reached1 = abs(lumaEnd1) >= gradientScaled;
    bool reached2 = abs(lumaEnd2) >= gradientScaled;
    bool reachedBoth = reached1 && reached2;

    // If the side is not reached, we continue to explore in this direction.
    if(!reached1){
        uv1 -= offset;
    }
    if(!reached2){
        uv2 += offset;
    }

    int ITERATIONS = 12;
    if(!reachedBoth){

        for(int i = 2; i < ITERATIONS; i++){
            if(!reached1){
                lumaEnd1 = rgb2luma(texture(myTexture, uv1).rgb);
                lumaEnd1 = lumaEnd1 - lumaLocalAverage;
            }
            if(!reached2){
                lumaEnd2 = rgb2luma(texture(myTexture, uv2).rgb);
                lumaEnd2 = lumaEnd2 - lumaLocalAverage;
            }
            reached1 = abs(lumaEnd1) >= gradientScaled;
            reached2 = abs(lumaEnd2) >= gradientScaled;
            reachedBoth = reached1 && reached2;

            if(!reached1){
                uv1 -= offset * QUALITY(i);
            }
            if(!reached2){
                uv2 += offset * QUALITY(i);
            }

            if(reachedBoth){ break;}
        }
    }

    float distance1 = isHorizontal ? (uvCoord.x - uv1.x) : (uvCoord.y - uv1.y);
    float distance2 = isHorizontal ? (uv2.x - uvCoord.x) : (uv2.y - uvCoord.y);

    bool isDirection1 = distance1 < distance2;
    float distanceFinal = min(distance1, distance2);

    float edgeThickness = (distance1 + distance2);

    float pixelOffset = - distanceFinal / edgeThickness + 0.5;

    bool isLumaCenterSmaller = lumaCenter < lumaLocalAverage;

    bool correctVariation = ((isDirection1 ? lumaEnd1 : lumaEnd2) < 0.0) != isLumaCenterSmaller;

    float finalOffset = correctVariation ? pixelOffset : 0.0;

    float lumaAverage = (1.0/12.0) * (2.0 * (lumaDownUp + lumaLeftRight) + lumaLeftCorners + lumaRightCorners);

    float subPixelOffset1 = clamp(abs(lumaAverage - lumaCenter)/lumaRange,0.0,1.0);
    float subPixelOffset2 = (-2.0 * subPixelOffset1 + 3.0) * subPixelOffset1 * subPixelOffset1;

    float SUBPIXEL_QUALITY = 0.75;
    float subPixelOffsetFinal = subPixelOffset2 * subPixelOffset2 * SUBPIXEL_QUALITY;

    finalOffset = max(finalOffset,subPixelOffsetFinal);

    vec2 finalUv = uvCoord;
    if(isHorizontal){
        finalUv.y += finalOffset * stepLength;
    } else {
        finalUv.x += finalOffset * stepLength;
    }

    vec3 finalColor = texture(myTexture,finalUv).rgb;
    fragColor = vec4(finalColor,1);
    return fragColor;
}

void main()
{
//    fragColor = vec4(1);
    // Task 17: Set fragColor using the sampler2D at the UV coordinate
    fragColor = texture(myTexture, uvCoord);
    // Task 33: Invert fragColor's r, g, and b color channels if your bool is true
    if (isPixelPostProcessingOn && !isKernelPostProcessingOn) {
        // invert
         fragColor = vec4(1) - fragColor;

        // gray scale extra credit
        // float inten = (fragColor.r + fragColor.g + fragColor.b) /3.f;
        // fragColor = vec4(vec3(inten),1);
    }

    float u = 1.f / textureWidth; // textureWidth is basically width of screen?
    float v = 1.f / textureHeight;

    /************** FXAA extra credit *****************/
    // fragColor = FXAA();
    /************** FXAA extra credit *****************/

    if (isKernelPostProcessingOn) {

        fragColor = vec4(0.f);

        vec2 uvCoord1 = vec2(uvCoord.x - u, uvCoord.y+v);
        vec2 uvCoord2 = vec2(uvCoord.x, uvCoord.y+u);
        vec2 uvCoord3 = vec2(uvCoord.x + u, uvCoord.y+v);

        vec2 uvCoord4 = vec2(uvCoord.x - u, uvCoord.y);
        vec2 uvCoord5 = vec2(uvCoord.x, uvCoord.y);
        vec2 uvCoord6 = vec2(uvCoord.x + u, uvCoord.y);

        vec2 uvCoord7 = vec2(uvCoord.x - u, uvCoord.y-v);
        vec2 uvCoord8 = vec2(uvCoord.x, uvCoord.y-u);
        vec2 uvCoord9 = vec2(uvCoord.x + u, uvCoord.y-v);

        if (isEdgeFilterOn) {
            fragColor += (filter1[0] * giveColor(uvCoord1));
            fragColor += (filter1[1] * giveColor(uvCoord2));
            fragColor += (filter1[2] * giveColor(uvCoord3));
            fragColor += (filter1[3] * giveColor(uvCoord4));
            fragColor += (filter1[4] * giveColor(uvCoord5));
            fragColor += (filter1[5] * giveColor(uvCoord6));
            fragColor += (filter1[6] * giveColor(uvCoord7));
            fragColor += (filter1[7] * giveColor(uvCoord8));
            fragColor += (filter1[8] * giveColor(uvCoord9));

        } else { // sharpen filter
            fragColor += (filter1[0]- 1.f/9 * filter2[0]) * giveColor(uvCoord1); // float * vec4

            fragColor += (filter1[1]- 1.f/9 * filter2[1]) * giveColor(uvCoord2);
            fragColor += (filter1[2]- 1.f/9 * filter2[2]) * giveColor(uvCoord3);
            fragColor += (filter1[3]- 1.f/9 * filter2[3]) * giveColor(uvCoord4);
            fragColor += (filter1[4]- 1.f/9 * filter2[4]) * giveColor(uvCoord5);
            fragColor += (filter1[5]- 1.f/9 * filter2[5]) * giveColor(uvCoord6);
            fragColor += (filter1[6]- 1.f/9 * filter2[6]) * giveColor(uvCoord7);
            fragColor += (filter1[7]- 1.f/9 * filter2[7]) * giveColor(uvCoord8);
            fragColor += (filter1[8]- 1.f/9 * filter2[8]) * giveColor(uvCoord9);

        }

        if (isPixelPostProcessingOn) {
            fragColor = vec4(1) - fragColor;
        }
    }

//    else if (isKernelPostProcessingOn && isPixelPostProcessingOn){
////        fragColor = texture(myTexture, uvCoord9가지경우);
////        fragColor = vec4(1) - fragColor; 이걸 밑에서 곱하기

//        fragColor = vec4(0.f);
//        float u = 1.f / textureWidth;
//        float v = 1.f / textureHeight;

//        vec2 uvCoord1 = vec2(uvCoord.x - u, uvCoord.y+v);
//        vec2 uvCoord2 = vec2(uvCoord.x, uvCoord.y+u);
//        vec2 uvCoord3 = vec2(uvCoord.x + u, uvCoord.y+v);

//        vec2 uvCoord4 = vec2(uvCoord.x - u, uvCoord.y);
//        vec2 uvCoord5 = vec2(uvCoord.x, uvCoord.y);
//        vec2 uvCoord6 = vec2(uvCoord.x + u, uvCoord.y);

//        vec2 uvCoord7 = vec2(uvCoord.x - u, uvCoord.y-v);
//        vec2 uvCoord8 = vec2(uvCoord.x, uvCoord.y-u);
//        vec2 uvCoord9 = vec2(uvCoord.x + u, uvCoord.y-v);

//        // multiply each element of filter by r,g,b of color at uv coordinate
////        for (int row = 0; )

////        for (int i = 0; i < 9; ++i) {
////            fragColors[i] = filter1[0] * giveColor(uvCoord);
////        }
//        fragColor += (filter1[0]- 1.f/9 * filter2[0]) * giveColor(uvCoord1); // float * vec4

//        fragColor += (filter1[1]- 1.f/9 * filter2[1]) * giveColor(uvCoord2);
//        fragColor += (filter1[2]- 1.f/9 * filter2[2]) * giveColor(uvCoord3);
//        fragColor += (filter1[3]- 1.f/9 * filter2[3]) * giveColor(uvCoord4);
//        fragColor += (filter1[4]- 1.f/9 * filter2[4]) * giveColor(uvCoord5);
//        fragColor += (filter1[5]- 1.f/9 * filter2[5]) * giveColor(uvCoord6);
//        fragColor += (filter1[6]- 1.f/9 * filter2[6]) * giveColor(uvCoord7);
//        fragColor += (filter1[7]- 1.f/9 * filter2[7]) * giveColor(uvCoord8);
//        fragColor += (filter1[8]- 1.f/9 * filter2[8]) * giveColor(uvCoord9);
//    }

}

