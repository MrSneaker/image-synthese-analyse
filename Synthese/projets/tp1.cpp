
#include </home/mateo/COURS/L3/LIFIMA/TP/Proj/projet-lifimage/Synthese/src/image.h>
#include </home/mateo/COURS/L3/LIFIMA/TP/Proj/projet-lifimage/Synthese/src/image_io.h>
#include </home/mateo/COURS/L3/LIFIMA/TP/Proj/projet-lifimage/Synthese/src/vec.h>
#include <limits>
#include <cmath>
#include <vector>
#include <random>
#include <time.h>

const float inf = std::numeric_limits<float>::infinity();

struct Hit
{
    float t;     // position sur le rayon ou inf
    Vector n;    // normale du point d'intersection, s'il existe
    Color color; // couleur du point d'intersection, s'il existe
    bool is_mirror;
    Color refract;
    operator bool() { return t >= 0 && t < inf; }
};

struct Source
{
    Point p;
    Color col;
};

struct sphere
{
    Color col;
    Point p;
    float r;
    Color refraction;
    bool is_miror;
};

struct plan
{
    Color col;
    Point a;
    Vector n;
    Color refraction;
    bool is_miror;
};

struct Scene
{
    std::vector<sphere> spheres;
    std::vector<Source> sources;
    plan p;
};

void genere_sources(std::vector<Source> &sources, const Point &a, const Vector &u, const Vector &v, const Color &emission, const int n)
{
    std ::random_device hwseed;
    unsigned seed = hwseed();
    std ::default_random_engine rng(seed);
    std ::uniform_real_distribution<float> uni;
    for (int i = 0; i < n; i++)
    {
        float u1 = uni(rng); // nombre aleatoire entre 0 et 1
        float u2 = uni(rng);
        // les coordonn´ees sont entre 0 et 1
        Source src;
        Point s = a + u1 * u + u2 * v;
        src.p = s;
        src.col = Color(emission / n, 1);
        sources.push_back(src);
    }
}

void genere_dome(int n, std ::vector<Vector> &directions)
{
    std ::random_device hwseed;
    unsigned seed = hwseed();
    std ::default_random_engine rng(seed);
    std ::uniform_real_distribution<float> u;

    for (int i = 0; i < n; i++)
    {
        float cos_theta = u(rng);
        float sin_theta = std ::sqrt(1 - cos_theta * cos_theta);
        float phi = u(rng) * float(M_PI * 2);
        Vector d = Vector(std ::cos(phi) * sin_theta,
                          cos_theta,
                          std ::sin(phi) * sin_theta);
        directions.push_back(d);
    }
}

Hit intersect_plan(/* parametres du plan */ const plan &p, /* parametres du rayon */ const Point &o, const Vector &d)
{
    // intersection avec le rayon o , d
    Hit h;
    float t = dot(p.n, Vector(o, p.a)) / dot(p.n, d);
    if (t < 0)
    {
        t = inf;
    }
    h.t = t;
    h.n = p.n;
    h.color = p.col;
    h.is_mirror = p.is_miror;
    h.refract = p.refraction;
    return h;
}

Hit intersect_sphere(/* parametres de la sphere */ const sphere &s, /* parametres du rayon */ const Point &o, const Vector &d)
{
    Hit h;
    float a = dot(d, d);
    float b = 2 * dot(d, Vector(s.p, o));
    float k = dot(Vector(s.p, o), Vector(s.p, o)) - (s.r * s.r);
    float t, t1, t2;
    float det = b * b - 4 * a * k;
    if (det < 0)
    {
        t = inf;
    }
    else
    {
        t1 = (-b + sqrt(det)) / (2 * a);
        t2 = (-b - sqrt(det)) / (2 * a);
        // std::cout << "t1 : " << t1 << "\nt2 : " << t2 << "\n";
        if (t1 < 0 && t2 < 0)
        {
            t = inf;
        }
        else if (t1 < 0)
        {
            t = t2;
        }
        else if (t2 < 0)
        {
            t = t1;
        }
        else
            t = std::min(t1, t2);
    }
    h.t = t;
    Point p = o + t * d;
    h.n = Vector(s.p, p);
    h.color = s.col;
    h.is_mirror = s.is_miror;
    h.refract = s.refraction;
    return h;
}

Hit intersect_aux(const Scene &scene, const Point &o, const Vector &d)
{
    Hit plus_proche;
    plus_proche.t = inf;
    for (const auto &source : scene.sources)
    {
        for (unsigned i = 0; i < scene.spheres.size(); i++)
        {
            // tester la ieme sphere
            Hit h = intersect_sphere(scene.spheres[i], o, d);
            if (h.t < plus_proche.t)
            {
                plus_proche.t = h.t;
                plus_proche.n = h.n;
                Color emission = source.col;
                Point p = o + plus_proche.t * d;
                Vector l = Vector(p, source.p);
                double cos_theta = std ::max(float(0), dot(normalize(plus_proche.n), normalize(l)));
                plus_proche.color = h.color;
                plus_proche.color = emission * plus_proche.color * cos_theta / (float)length2(l);
                plus_proche.color.a = 1;
                plus_proche.is_mirror = h.is_mirror;
            }
        }

        // et bien sur, on n'oublie pas le plan...
        Hit h = intersect_plan(scene.p, o, d);
        if (h.t < plus_proche.t)
        {
            plus_proche.t = h.t;
            plus_proche.n = h.n;
            plus_proche.color = h.color;
            Color emission = source.col;
            Point p = o + plus_proche.t * d;
            Vector l = Vector(p, source.p);
            double cos_theta = std ::max(float(0), dot(normalize(plus_proche.n), normalize(l)));
            plus_proche.color = emission * plus_proche.color * cos_theta / (float)length2(l);
            plus_proche.color.a = 1;
            plus_proche.is_mirror = h.is_mirror;
        }
    }
    return plus_proche;
}

Vector reflect(const Vector &n, const Vector &v)
{
    return v - 2 * dot(n, v) * n;
}

bool isInShadow(const Scene &scene, const Point &p, const Vector &d)
{
    Hit ombre_intersection = intersect_aux(scene, p, d);
    bool ombre = ombre_intersection.t > 0 && ombre_intersection.t < 1;
    return ombre;
}

Color eclairage_direct(const Point &p, const Vector &n, const Color &color, const Scene &scene, const Vector &d, bool is_mir, Color refract, int depth)
{
    // std::cout << "depth : " << depth << std::endl;
    Color res = Black();
    bool dans_ombre = false;
    double epsilon = 0.001;
    for (const auto &s : scene.sources)
    {
        Point o_ombre = p + epsilon * n;
        Vector d_ombre(o_ombre, s.p);
        dans_ombre = isInShadow(scene, o_ombre, d_ombre);
        if (dans_ombre && !is_mir)
            continue;
        Vector l = Vector(p, s.p);
        double cos_theta = std ::max(float(0), dot(normalize(n), normalize(l)));

        if (is_mir)
        {
            Point o_mir = p + epsilon * n;
            Vector m = reflect(n, d);
            Hit mir = intersect_aux(scene, o_mir, m);
            if (mir)
            {
                if (((depth > 4) || !mir.is_mirror))
                {
                    if (mir.is_mirror)
                        continue;
                    Color ref = refract + (White() - refract) * pow((1 - cos_theta), 5);
                    res = res + Color(ref.r * mir.color.r, ref.g * mir.color.g, ref.b * mir.color.b);
                }
                else if (mir.is_mirror)
                {
                    Point mir_p = o_mir + mir.t * m;
                    res = eclairage_direct(mir_p, mir.n, color, scene, m, true, mir.refract, depth + 1);
                }
            }
        }
        else
        {
            res = res + s.col * color * cos_theta / (float)length2(l);
            res.a = 1;
            Point o_ombre = p + epsilon * n;
            Vector d_ombre(o_ombre, s.p);
            Hit ombre_intersection = intersect_aux(scene, o_ombre, d_ombre);
            bool ombre = ombre_intersection.t > 0 && ombre_intersection.t < 1;
            if (ombre)
            {
                res = Black();
            }
        }
    }
    return res;
}

Hit intersect(const Scene &scene, const Point &o, const Vector &d, int depth)
{
    Hit plus_proche;
    plus_proche.t = inf;
    for (unsigned i = 0; i < scene.spheres.size(); i++)
    {
        // tester la ieme sphere
        Hit h = intersect_sphere(scene.spheres[i], o, d);
        if (h.t < plus_proche.t)
        {
            plus_proche.t = h.t;
            plus_proche.n = h.n;
            Point p = o + plus_proche.t * d;
            if (scene.spheres[i].is_miror)
            {
                Color refract = scene.spheres[i].refraction;
                plus_proche.color = plus_proche.color + eclairage_direct(p, plus_proche.n, h.color, scene, d, true, refract, depth);
            }
            else
            {
                plus_proche.color = plus_proche.color + eclairage_direct(p, plus_proche.n, h.color, scene, d, false, Black(), depth);
            }
        }
    }

    // et bien sur, on n'oublie pas le plan...
    Hit h = intersect_plan(scene.p, o, d);
    if (h.t < plus_proche.t)
    {
        plus_proche.t = h.t;
        plus_proche.n = h.n;
        Point p = o + plus_proche.t * d;
        if (scene.p.is_miror)
        {
            Color refract = scene.p.refraction;
            plus_proche.color = eclairage_direct(p, plus_proche.n, h.color, scene, d, true, refract, depth + 1);
        }
        else
        {
            plus_proche.color = eclairage_direct(p, plus_proche.n, h.color, scene, d, false, Black(), depth);
        }
    }
    return plus_proche;
}

int main()
{
    std::cout << "process..." << std::endl;
    // rayon
    Point o = Point(0, 0, 0);

    // cree l'image resultat
    Image image(1024, 1024); // par exemple...
    sphere s, s1, s2, s3;
    s.r = 1;
    s.p = Point(-0.5, 1, -2.5);
    s.col = Color(1, 0, 0);
    s.is_miror = true;
    s.refraction = Color(0.98, 0.82, 0.76);

    s1.r = 0.5;
    s1.p = Point(-1.5, 0, -1.5);
    s1.is_miror = false;
    s1.col = Color(0.6, 0.3, 0.2);
    s1.refraction = Color(0, 0, 0);

    s2.r = 0.3;
    s2.p = Point(-0.75, 0.1, -1.9);
    s2.col = White();
    s2.is_miror = false;
    s2.refraction = Color(0, 0, 0);

    s3.is_miror = true;
    s3.col = Color(1, 0, 0);
    s3.p = Point(1, 0.4, -2);
    s3.refraction = Color(0.98, 0.82, 0.76);
    s3.r = 1;

    plan p2;
    p2.col = Color(0.5, 0.5, 0.5);
    p2.a = Point(0, -1, 0);
    p2.n = Vector(0, 1, 0);
    p2.refraction = Color(0.98, 0.82, 0.76);
    p2.is_miror = true;

    Source src, src2, src3;
    src.p = Point(-0.5, 0.75, -3);
    src.col = Color(1, 1, 1);
    src2.p = Point(0, 0.75, -2);
    src2.col = White();
    Scene sc;
    sc.spheres.push_back(s);
    sc.spheres.push_back(s1);
    sc.spheres.push_back(s2);
    sc.spheres.push_back(s3);

    genere_sources(sc.sources, o, Vector(1, 0, 0), Vector(0, 1, 0), White(), 10);
    sc.sources.push_back(src);
    sc.sources.push_back(src2);

    sc.p = p2;
#pragma omp parallel for schedule(dynamic, 1)
    for (int py = 0; py < image.height(); py++)
        for (int px = 0; px < image.width(); px++)
        {
            // std::cout << "px-py : " << px << " - " << py << std::endl;
            std::default_random_engine rng;
            std::uniform_real_distribution<float> u;
            int aa = 5;
            Color color;
            for (int pa = 0; pa < aa; pa++)
            {
                float ux = u(rng);
                float uy = u(rng);
                Point o = Point(0, 0, 0); // origine
                float aspect_ratio = (float)image.width() / image.height();
                float y = (2.0 * (py + uy) / image.height() - 1.0);
                float x = (2.0 * (px + ux) / image.width() - 1.0) * aspect_ratio;
                Point e(x, y, -1);       // extremite
                Vector d = Vector(o, e); // direction : extremite - origine
                Hit plus_proche;
                plus_proche.t = inf;
                plus_proche.color = color;
                Hit intersection;
                intersection = intersect(sc, o, d, 0);
                if (intersection)
                {
                    color = color + intersection.color;
                }
            }
            image(px, py) = Color(color / aa, 1);
        }
    write_image_hdr(image, "image.hdr");
    return 0;
}
