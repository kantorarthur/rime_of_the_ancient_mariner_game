uniform float wingFlap;

const float WING_SHOULDER = 0.16;
const float WING_WRIST = 0.8;

mat2 rotation(float angle)
{
    float c = cos(angle);
    float s = sin(angle);
    return mat2(c, s, -s, c);
}

void flapWings(inout vec3 position, inout vec3 normal)
{
    if(wingFlap == 0.0)
        return;

    float side = position.x < 0.0 ? -1.0 : 1.0;
    vec2 point = vec2(abs(position.x), position.y);
    vec2 pointNormal = vec2(normal.x * side, normal.y);

    float shoulderAngle = wingFlap * smoothstep(WING_SHOULDER, WING_SHOULDER + 0.12, point.x);
    float wristAngle = 0.5 * wingFlap * smoothstep(WING_WRIST, WING_WRIST + 0.15, point.x);

    vec2 shoulder = vec2(WING_SHOULDER, 0.0);
    vec2 wrist = shoulder + rotation(shoulderAngle) * vec2(WING_WRIST - WING_SHOULDER, 0.0);
    point = shoulder + rotation(shoulderAngle) * (point - shoulder);
    point = wrist + rotation(wristAngle) * (point - wrist);
    pointNormal = rotation(shoulderAngle + wristAngle) * pointNormal;

    position.x = point.x * side;
    position.y = point.y;
    normal.x = pointNormal.x * side;
    normal.y = pointNormal.y;
}
