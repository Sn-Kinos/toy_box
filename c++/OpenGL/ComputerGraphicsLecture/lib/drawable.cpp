#include<cstdlib>
#include<cmath>
#include<vector>
#include<GL/glew.h>

#include<glm/glm.hpp>
// #include<glm/gtc/matrix_transform.hpp>
using namespace glm;

#include "drawable.hpp"

Drawable::Drawable(int meshCnt) {
    this->meshCnt = meshCnt;
    this->vertexCnt = meshCnt * 3;
    this->drawMode = GL_TRIANGLES;
    this->vertex = NULL;
    this->color = NULL;
    this->normal = NULL;
    this->centerPoint = vec3(0.0f, 0.0f, 0.0f);
}

Drawable::~Drawable() {
}

std::vector<vec3> Drawable::getCirclePoints(float rad, vec3 center, int cnt) {
    float delta = M_PI * 2.0 / cnt;
    std::vector<vec3> circlePoints;
    for (int i = 0; i <= cnt; i++) {
        float circlePoint_x = rad * cos(i * delta) + center.x;
        float circlePoint_y = center.y;
        float circlePoint_z = rad * sin(i * delta) + center.z;
        circlePoints.push_back(vec3(circlePoint_x, circlePoint_y, circlePoint_z));
    }
    return circlePoints;
}

void Drawable::setCircle(float rad) {
    this->drawMode = GL_TRIANGLE_FAN;
    this->vertexCnt = this->meshCnt + 2;
    int arraySize = this->vertexCnt * 3;

    this->vertex = new GLfloat[arraySize];
    this->vertex[0] = 0.0f; this->vertex[1] = 0.0f; this->vertex[2] = 0.0f;
    std::vector<vec3> circlePoints = this->getCirclePoints(rad, vec3(0.0f, 0.0f, 0.0f), this->meshCnt);
    std::vector<vec3>::iterator it = circlePoints.begin();
    for (int i = 1; it != circlePoints.end(); it++, i++) {
        this->vertex[i*3] = (*it).x;
        this->vertex[i*3+1] = (*it).y;
        this->vertex[i*3+2] = (*it).z;
    }
}

void Drawable::setCylinder(float rad, float height, int heightLevel, int circleFlag) {
    this->drawMode = GL_TRIANGLES;
    // top circle, bottom circle, side mesh * 2
    int circleMeshCnt;
    if(circleFlag == DRAWABLE_CIRCLEFLAG_CIRCLE) {
        circleMeshCnt = 2;
    } else {
        circleMeshCnt = 0;
    }
    this->vertexCnt = this->meshCnt * (circleMeshCnt + heightLevel*2) * 3;
    int arraySize = this->vertexCnt * 3;

    this->vertex = new GLfloat[arraySize];
    vec3 originPoint = vec3(0.0f, 0.0f, 0.0f);
    std::vector<vec3> circlePoints = this->getCirclePoints(rad, originPoint, this->meshCnt);

    int idx = 0;
    vec3 heightLevelVec = vec3(0.0f, height / heightLevel, 0.0f);
    vec3 heightVec = vec3(0.0f, height, 0.0f);
    vec3 bottomOriginPoint = originPoint - heightVec;
    std::vector<vec3>::iterator it = circlePoints.begin();
    std::vector<vec3>::iterator prevIt = it;
    for(it++; it != circlePoints.end(); it++, prevIt++) {
        if (circleFlag == DRAWABLE_CIRCLEFLAG_CIRCLE) {
            // top circle vertex
            this->vertex[idx*3] = originPoint.x; this->vertex[idx*3+1] = originPoint.y; this->vertex[idx*3+2] = originPoint.z; idx++;
            this->vertex[idx*3] = (*prevIt).x; this->vertex[idx*3+1] = (*prevIt).y; this->vertex[idx*3+2] = (*prevIt).z; idx++;
            this->vertex[idx*3] = (*it).x; this->vertex[idx*3+1] = (*it).y; this->vertex[idx*3+2] = (*it).z; idx++;

            vec3 bottomVertex = (*it) - heightVec;
            vec3 bottomPrevVertex = (*prevIt) - heightVec;

            // bottom circle vertex
            this->vertex[idx*3] = bottomOriginPoint.x; this->vertex[idx*3+1] = bottomOriginPoint.y; this->vertex[idx*3+2] = bottomOriginPoint.z; idx++;
            this->vertex[idx*3] = bottomPrevVertex.x; this->vertex[idx*3+1] = bottomPrevVertex.y; this->vertex[idx*3+2] = bottomPrevVertex.z; idx++;
            this->vertex[idx*3] = bottomVertex.x; this->vertex[idx*3+1] = bottomVertex.y; this->vertex[idx*3+2] = bottomVertex.z; idx++;
        }

        for(int j = 0; j < heightLevel; j++) {
            vec3 jVec = vec3(j, j, j);
            vec3 jNextVec = vec3(j+1, j+1, j+1);
            vec3 topVertex = (*it) - jVec * heightLevelVec;
            vec3 topPrevVertex = (*prevIt) - jVec * heightLevelVec;
            vec3 bottomVertex = (*it) - jNextVec * heightLevelVec;
            vec3 bottomPrevVertex = (*prevIt) - jNextVec * heightLevelVec;

            // side upper wall vertex
            this->vertex[idx*3] = topPrevVertex.x; this->vertex[idx*3+1] = topPrevVertex.y; this->vertex[idx*3+2] = topPrevVertex.z; idx++;
            this->vertex[idx*3] = topVertex.x; this->vertex[idx*3+1] = topVertex.y; this->vertex[idx*3+2] = topVertex.z; idx++;
            this->vertex[idx*3] = bottomVertex.x; this->vertex[idx*3+1] = bottomVertex.y; this->vertex[idx*3+2] = bottomVertex.z; idx++;

            // side lower wall vertex
            this->vertex[idx*3] = topPrevVertex.x; this->vertex[idx*3+1] = topPrevVertex.y; this->vertex[idx*3+2] = topPrevVertex.z; idx++;
            this->vertex[idx*3] = bottomPrevVertex.x; this->vertex[idx*3+1] = bottomPrevVertex.y; this->vertex[idx*3+2] = bottomPrevVertex.z; idx++;
            this->vertex[idx*3] = bottomVertex.x; this->vertex[idx*3+1] = bottomVertex.y; this->vertex[idx*3+2] = bottomVertex.z; idx++;
        }
    }
    this->translate(heightVec / 2.0f);
    this->setCenterPoint(vec3(0.0f, 0.0f, 0.0f));
}

void Drawable::setSphere(float rad, int halfFlag) {
    this->drawMode = GL_TRIANGLE_STRIP;
    float y_delta = M_PI * 2.0 / this->meshCnt;
    int halfIdx;
    int ySize;
    if (halfFlag == DRAWABLE_HALFFLAG_SPHERE) {
        halfIdx = 0;
        ySize = this->meshCnt / 2;
    } else if (halfFlag == DRAWABLE_HALFFLAG_UP) {
        halfIdx = 0;
        ySize = this->meshCnt / 4 + 1;
    } else if(halfFlag == DRAWABLE_HALFFLAG_DOWN) {
        halfIdx = this->meshCnt / 4;
        ySize = this->meshCnt / 2;
    }

    this->vertexCnt = (this->meshCnt + 1) * (ySize-halfIdx) * 2 + 1;
    int arraySize = this->vertexCnt * 3;

    this->vertex = new GLfloat[arraySize];
    std::vector< std::vector<vec3> > levelPoints;
    for(int i = halfIdx; i <= ySize; i++) {
        float y = rad * cos(i * y_delta);
        std::vector<vec3> circlePoints = this->getCirclePoints(sqrt(rad*rad - y*y), vec3(0.0f, y, 0.0f), this->meshCnt);
        levelPoints.push_back(circlePoints);
    }

    int j = 0;
    std::vector< std::vector<vec3> >::iterator prev_it, it;
    prev_it = levelPoints.begin();
    it = prev_it + 1;
    for(; it != levelPoints.end(); prev_it++, it++) {
        std::vector<vec3>::iterator prev_point_it, point_it;
        for(prev_point_it = (*prev_it).begin(), point_it = (*it).begin();
            point_it != (*it).end();
            prev_point_it++, point_it++) {
                this->vertex[j*3] = (*prev_point_it).x; this->vertex[j*3+1] = (*prev_point_it).y; this->vertex[j*3+2] = (*prev_point_it).z; j++;
                this->vertex[j*3] = (*point_it).x; this->vertex[j*3+1] = (*point_it).y; this->vertex[j*3+2] = (*point_it).z; j++;
        }
    }
    // degenerate triangle
    this->vertex[j*3] = this->vertex[(j-1)*3]; this->vertex[j*3+1] = this->vertex[(j-1)*3+1]; this->vertex[j*3+2] = this->vertex[(j-1)*3+2]; j++;
}

void Drawable::setColor(vec3 c) {
    this->color = new GLfloat[this->vertexCnt * 3];
    for (int i = 0; i < this->vertexCnt; i++) {
        this->color[i*3] = c.r;
        this->color[i*3+1] = c.g;
        this->color[i*3+2] = c.b;
    }
}

void Drawable::setNormal(vec3 n) {
    this->normal = new GLfloat[this->vertexCnt * 3];
    for (int i = 0; i < this->vertexCnt; i++) {
        this->normal[i*3] = n.x;
        this->normal[i*3+1] = n.y;
        this->normal[i*3+2] = n.z;
    }
}

void Drawable::setCenterPoint(vec3 o) {
    this->centerPoint = o;
}

void Drawable::translate(vec3 v) {
    for (int i = 0; i < this->vertexCnt; i++) {
        this->vertex[i*3] = this->vertex[i*3] + v.x;
        this->vertex[i*3+1] = this->vertex[i*3+1] + v.y;
        this->vertex[i*3+2] = this->vertex[i*3+2] + v.z;
    }
    this->centerPoint = this->centerPoint + v;
}

void Drawable::scale(vec3 factor) {
    for (int i = 0; i < this->vertexCnt; i++) {
        this->vertex[i*3] = this->centerPoint.x + (this->vertex[i*3] - this->centerPoint.x) * factor.x;
        this->vertex[i*3+1] = this->centerPoint.y + (this->vertex[i*3+1] - this->centerPoint.y) * factor.y;
        this->vertex[i*3+2] = this->centerPoint.z + (this->vertex[i*3+2] - this->centerPoint.z) * factor.z;
    }
}

void Drawable::rotate(vec3 rad) {
    mat3 rx = mat3(1.0f, 0.0f, 0.0f,
                    0.0f, cos(rad.x), -1*sin(rad.x),
                    0.0f, sin(rad.x), cos(rad.x));
    mat3 ry = mat3(cos(rad.y), 0.0f, sin(rad.y),
                    0.0f, 1.0f, 0.0f,
                    -1*sin(rad.y), 0.0f, cos(rad.y));
    mat3 rz = mat3(cos(rad.z), -1*sin(rad.z), 0.0f,
                    sin(rad.z), cos(rad.z), 0.0f,
                    0.0f, 0.0f, 1.0f);

    mat3 r = rz * ry * rx;
    for (int i = 0; i < this->vertexCnt; i++) {
        vec3 v = vec3(this->vertex[i*3], this->vertex[i*3+1], this->vertex[i*3+2]);
        vec3 result = this->centerPoint + r * v;
        this->vertex[i*3] = result.x;
        this->vertex[i*3+1] = result.y;
        this->vertex[i*3+2] = result.z;
    }
}