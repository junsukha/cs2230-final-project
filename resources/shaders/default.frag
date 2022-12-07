#version 330 core
/** fragment shader colors each vertex**/

// texture mapping ec
in vec2 uvCoord; // get this from defaut.vert
uniform sampler2D myTexture2;
// need to use something like: textureColor = texture(myTexture, uvCoord); and blend it with light color
struct Light{
    vec3 function;
    vec4 color;
    vec4 position; // world space
    bool isDirectional;
    bool isSpotLight;
    vec4 direction;
    float penumbra;
    float angle;
};

uniform Light lights[8];
//int numOfLights = 8;
uniform int currentNumOfLights;

// variables I need to get from vertex shader. Variables that are different for each vertex
in vec4 worldSpacePosition;
in vec4 worldSpaceNormal;

uniform vec4 worldSpaceCameraPosition;
// not sure
// since, although k_a is the same across vertices, cAmbient for each objects is different. So can't sent this as uniform. should come from vertex shader
//

out vec4 fragColor;

/** uniform varialbes are the same for all vertices **/

// ambient lighting scene coefficient
uniform float k_a;

// diffuse lighting scene coefficient
uniform float k_d;

// specular lighting scene coefficient
uniform float k_s;

// vary by object. but same for all vertices
uniform float shininess;
uniform vec4 o_a;
uniform vec4 o_d;
uniform vec4 o_s;
uniform vec3 function;
uniform float blend;

// although it vaires for each light, it's the same for all vertices for each light
// there will be several uniform light position given
uniform vec4 worldSpaceLightPosition;
uniform vec4 directionLight; // direction of directional light

//uniform vec4 worldspaceCameraPosition;

uniform int numOfLights;

// for spot light
float calculateFallOff(Light light, float x) {
    float outer = light.angle;
    float inner =  light.angle -light.penumbra;
    return -2 * pow((x - inner)/(outer-inner),3) + 3 * pow((x - inner) / (outer - inner),2);
}

// for spot light
vec4 calculateSpotLightIntensity(vec4 surfacePosition, Light light) {// position is in worldspace
    vec4 currentDirectionFromLightToPos = surfacePosition - light.position;
    float dotProd = dot(normalize(currentDirectionFromLightToPos),
                             normalize(light.direction));
    float x = acos(dotProd);// angle (radian) between current direction and spotlight direction

    float inner = light.angle - light.penumbra;
    float outer = light.angle;
    if (x <= inner) {
        // return full light intensity
        return light.color;
    } else if (x > inner && x < outer){
        return light.color * (1.f - calculateFallOff(light, x));
    } else {
        return light.color * 0.f;
    }
}

float time = 1e-4;
mat4 rotateCCWZ;

void main() {
//    rotateCCWZ[0] = vec4(cos(time), sin(time), 0, 0.f);
//    rotateCCWZ[1] = vec4(-sin((time)), cos((time)), 0, 0.f);
//    rotateCCWZ[2] = vec4(0,0,1,0.f);
//    rotateCCWZ[3] = vec4(0,0,0,1.f);
//    uvCoord = rotateCCWZ * uvCoord;

    // Remember that you need to renormalize vectors here if you want them to be normalized

    // Task 10: set your output color to white (i.e. vec4(1.0)). Make sure you get a white circle!
    // fragColor = vec4(1.0);
    // Task 11: set your output color to the absolute value of your world-space normals,
    //          to make sure your normals are correct.
    // fragColor = abs(worldSpaceNormal);
    fragColor = vec4(0.0);
    // Task 12: add ambient component to output color
    fragColor = k_a * o_a;

    // final project. texture mapping


    vec4 texColor = texture(myTexture2, uvCoord);

    vec4 intensity;
    vec4 fromSurfaceToLightWorldSpace;
    float dist;
    float ftt;
    for (int i = 0; i < numOfLights; ++i) {

        if (!lights[i].isDirectional) { // light is not directional light
            fromSurfaceToLightWorldSpace = lights[i].position - worldSpacePosition;

            // get distance from surface to light for attenuation
            dist = max(0,distance(lights[i].position, worldSpacePosition));
            if (dist == 0)
                dist = 0.0001;

            ftt = min(1.0, 1.0/(lights[i].function[0] + dist * lights[i].function[1] + pow(dist,2) * lights[i].function[2]));

        }
        else if (lights[i].isDirectional){ // light is directional light
            fromSurfaceToLightWorldSpace = -lights[i].direction;
            ftt = 1.f;
        }

        if (lights[i].isSpotLight) {
            intensity = calculateSpotLightIntensity(worldSpacePosition, lights[i]);
        }else { // point light and directional light
            intensity = lights[i].color;
        }

        // Texture mapping ec
        // vec4 texturColor = texture(myTexture, uvCoord);
        // blend textureColor and fragColor

        // Task 13: add diffuse component to output color
        // since light position of light direction is the same for all vertices,
        float diffuse = clamp(dot(normalize(worldSpaceNormal), normalize(fromSurfaceToLightWorldSpace)), 0.f, 1.f);

        fragColor += ftt * intensity * k_d * o_d * diffuse;

        // Task 14: add specular component to output color
        vec4 fromSurfaceToCameraWorldSpace = normalize(worldSpaceCameraPosition - worldSpacePosition);
        // reflect(incident vector, surface normal vector).. incident vector = fromLightToSurface
        vec4 fromLightToSurfaceWorldspace = normalize(-fromSurfaceToLightWorldSpace);
        vec4 reflected = reflect(normalize(fromLightToSurfaceWorldspace), normalize(worldSpaceNormal));
        // remember to normalize when using dot product
        float RE = clamp(dot(normalize(reflected), normalize(fromSurfaceToCameraWorldSpace)), 0.f, 1.f);
//        if (shininess == 0.f)
//            shininess = 1e-3;
        if (RE == 0.f)
            RE = 1e-5;

        fragColor += ftt * intensity * k_s * o_s * pow(RE, shininess);
    }

    // uncomment this when settings.texture == true
    fragColor = blend*texColor + (1-blend)*fragColor;
}
