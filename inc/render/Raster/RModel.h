/***************
 * THECURLE    *
 *     STENCIL *
 ***************/

#include <utility>
#include <vector>
#include <core/Tuple.hpp>
#include <render/Geometry.h>
#include "RTriangles.h"

#pragma once

namespace Raster {
    struct ClipPlane {
        Vector normal;
        float distance;

        Point intersect(const Point& pA, const Point& pB) const {
            Vector diff = (pA - pB);
            double t = (-distance - (normal.x * pA.x + normal.y * pA.y + normal.z * pA.z)) / (normal.x * diff.x + normal.y * diff.y + normal.z * diff.z);
            return pA + ( diff * t );
        }
    };

    struct Mesh {
        std::vector<Point> vertices;
        std::vector<Tri> tris;
    };

    struct Model : public Geo {
        Mesh mesh;

        Model(Mesh m, const Matrix& t) : Geo(), mesh(std::move(m)) {
            setMatrix(t);
        }

        Vector normalAt(const Point& p) override { return { 0, 0, 0 }; }

    };

    class RenderableModel : public Model {

    public:
        Point boundingCenter;
        float boundingRadius;

        bool cameraSpace = false;
        bool rejected = false;

        std::vector<Point> translatedVertices;
        std::vector<Tri> clippedTris;

        // During clipping, we need to store the triangles and points to add separately
        // TODO: do we?
        std::vector<Point> newVertices;
        std::vector<Tri> newTris;

        RenderableModel(Model& m) : Model(m), boundingCenter({ 0, 0, 0 }), boundingRadius((float) std::sqrt(3)) {
            clippedTris = m.mesh.tris;
            translatedVertices.clear();
        }

        void applyTransform(Matrix& m) {
            for (size_t idx = 0; idx < mesh.vertices.size(); idx++) {
                translatedVertices[idx] = (Point) (m * translatedVertices[idx]);
            }

            boundingCenter = (Point) ( m * boundingCenter );

            cameraSpace = true;
        }

        void generateWorldSpace() {
            size_t vertexCount = mesh.vertices.size();
            translatedVertices.clear();
            translatedVertices.resize(vertexCount);

            Point newP;
            Matrix& wSpace = inverseTransform;

            for (size_t idx = 0; idx < mesh.vertices.size(); idx++) {
                newP = Point(mesh.vertices[idx]);
                newP = (Point) ( wSpace * newP );
                this->translatedVertices[idx] = newP;
            }
            boundingCenter = (Point)( inverseTransform * boundingCenter );

            cameraSpace = false;
        }

        void reject() {
            rejected = true;
        }

        std::list<Tri> clipTriangle(Tri t, std::vector<Point> v, const ClipPlane& p);

        void clipAgainstPlane(ClipPlane& p) {
            translatedVertices.insert(translatedVertices.end(), mesh.vertices.begin(), mesh.vertices.end());

            for (auto iter = clippedTris.begin(); iter != clippedTris.end(); iter++) {
                auto clipped = clipTriangle(*iter, translatedVertices, p);
                if (!clipped.empty())
                    newTris.insert(newTris.end(), clipped.begin(), clipped.end());
                else {
                    mesh.tris.erase(iter);
                    iter--;
                }
            }

            clippedTris.insert(clippedTris.end(), newTris.begin(), newTris.end());
        }
    };
}