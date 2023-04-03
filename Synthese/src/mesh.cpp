
#include <cstdio>
#include <cassert>
#include <string>
#include <algorithm>

#include "vec.h"
#include "mesh.h"

// definit les attributs du prochain sommet
Mesh& Mesh::default_color( const Color& color )
{
    m_color= color;
    return *this;
}

Mesh& Mesh::color( const vec4& color )
{
    if(m_colors.size() <= m_positions.size())
        m_colors.push_back(color);
    else
        m_colors.back()= color;
    return *this;
}

Mesh& Mesh::normal( const vec3& normal )
{
    if(m_normals.size() <= m_positions.size())
        m_normals.push_back(normal);
    else
        m_normals.back()= normal;
    return *this;
}

Mesh& Mesh::texcoord( const vec2& uv )
{
    if(m_texcoords.size() <= m_positions.size())
        m_texcoords.push_back(uv);
    else
        m_texcoords.back()= uv;
    return *this;
}

// insere un nouveau sommet
unsigned Mesh::vertex( const vec3& position )
{
    m_positions.push_back(position);

    // copie les autres attributs du sommet, uniquement s'ils sont definis
    if(m_texcoords.size() > 0 && m_texcoords.size() != m_positions.size())
        m_texcoords.push_back(m_texcoords.back());
    if(m_normals.size() > 0 && m_normals.size() != m_positions.size())
        m_normals.push_back(m_normals.back());
    if(m_colors.size() > 0 && m_colors.size() != m_positions.size())
        m_colors.push_back(m_colors.back());

    // copie la matiere courante, uniquement si elle est definie
    if(m_triangle_materials.size() > 0 && int(m_triangle_materials.size()) < triangle_count())
        m_triangle_materials.push_back(m_triangle_materials.back());
    
    unsigned index= m_positions.size() -1;
    // renvoie l'indice du sommet
    return index;
}

// update attributes
Mesh& Mesh::color( const unsigned id, const vec4& c )
{
    assert(id < m_colors.size());
    m_colors[id]= c;
    return *this;
}

Mesh& Mesh::normal( const unsigned id, const vec3& n )
{
    assert(id < m_normals.size());
    m_normals[id]= n;
    return *this;
}

Mesh& Mesh::texcoord( const unsigned id, const vec2& uv )
{
    assert(id < m_texcoords.size());
    m_texcoords[id]= uv;
    return *this;
}

void Mesh::vertex( const unsigned id, const vec3& p )
{
    assert(id < m_positions.size());
    m_positions[id]= p;
}

void Mesh::clear( )
{
    m_positions.clear();
    m_texcoords.clear();
    m_normals.clear();
    m_colors.clear();
    m_indices.clear();
    //~ m_materials.clear();
    m_triangle_materials.clear();
}

//
Mesh& Mesh::triangle( const unsigned a, const unsigned b, const unsigned c )
{
    assert(a < m_positions.size());
    assert(b < m_positions.size());
    assert(c < m_positions.size());
    m_indices.push_back(a);
    m_indices.push_back(b);
    m_indices.push_back(c);
    
    // copie la matiere courante, uniquement si elle est definie
    if(m_triangle_materials.size() > 0 && int(m_triangle_materials.size()) < triangle_count())
        m_triangle_materials.push_back(m_triangle_materials.back());
    
    return *this;
}

Mesh& Mesh::triangle_last( const int a, const int b, const int c )
{
    assert(a < 0);
    assert(b < 0);
    assert(c < 0);
    m_indices.push_back(int(m_positions.size()) + a);
    m_indices.push_back(int(m_positions.size()) + b);
    m_indices.push_back(int(m_positions.size()) + c);
    
    // copie la matiere courante, uniquement si elle est definie
    if(m_triangle_materials.size() > 0 && int(m_triangle_materials.size()) < triangle_count())
        m_triangle_materials.push_back(m_triangle_materials.back());
    
    return *this;
}

//~ Mesh& Mesh::restart_strip( )
//~ {
    //~ m_indices.push_back(~0u);   // ~0u plus grand entier non signe representable, ou UINT_MAX...
    //~ return *this;
//~ }

Mesh& Mesh::index( const int a )
{
    if(a < 0)
        m_indices.push_back(int(m_positions.size()) + a);
    else if(a < int(m_positions.size()))
        m_indices.push_back(a);
    else
    {
        printf("[error] Mesh::index(): invalid index...\n");
        return *this;   // erreur
    }
    
    // copie la matiere courante, uniquement si elle est definie
    if(m_triangle_materials.size() > 0 && int(m_triangle_materials.size()) < triangle_count())
        m_triangle_materials.push_back(m_triangle_materials.back());
    
    return *this;
}


Materials& Mesh::materials( )
{
    return m_materials;
}

const Materials& Mesh::materials( ) const
{
    return m_materials;
}

void Mesh::materials( const Materials& materials )
{
    m_materials= materials;
}

Mesh& Mesh::material( const unsigned id )
{
    if(int(m_triangle_materials.size()) <= triangle_count())
        m_triangle_materials.push_back(id);
    else
        m_triangle_materials.back()= id;
    
    return *this;
}

const std::vector<unsigned int>& Mesh::material_indices( ) const
{
    return m_triangle_materials;
}

int Mesh::triangle_material_index( const unsigned id ) const
{
    assert((size_t) id < m_triangle_materials.size());
    return m_triangle_materials[id];
}

const Material &Mesh::triangle_material( const unsigned id ) const
{
    assert((size_t) id < m_triangle_materials.size());
    return m_materials.material(m_triangle_materials[id]);
}

int Mesh::triangle_count( ) const
{
    if(m_indices.size() > 0)
        return int(m_indices.size() / 3);
    else
        return int(m_positions.size() / 3);
}

TriangleData Mesh::triangle( const unsigned int id ) const
{
    unsigned int a, b, c;
    if(m_indices.size() > 0)
    {
        assert((size_t) id*3+2 < m_indices.size());
        a= m_indices[id*3];
        b= m_indices[id*3 +1];
        c= m_indices[id*3 +2];
    }
    else
    {
        assert((size_t) id*3+2 < m_positions.size());
        a= id*3;
        b= id*3 +1;
        c= id*3 +2;
    }
    
    TriangleData triangle;
    triangle.a= m_positions[a];
    triangle.b= m_positions[b];
    triangle.c= m_positions[c];
    
    if(m_normals.size() == m_positions.size())
    {
        triangle.na= m_normals[a];
        triangle.nb= m_normals[b];
        triangle.nc= m_normals[c];
    }
    else
    {
        // calculer la normale geometrique
        Vector ab= Point(m_positions[b]) - Point(m_positions[a]);
        Vector ac= Point(m_positions[c]) - Point(m_positions[a]);
        Vector n= normalize(cross(ab, ac));
        triangle.na= vec3(n);
        triangle.nb= vec3(n);
        triangle.nc= vec3(n);
    }
    
    if(m_texcoords.size() == m_positions.size())
    {
        triangle.ta= m_texcoords[a];
        triangle.tb= m_texcoords[b];
        triangle.tc= m_texcoords[c];
    }
    else
    {
        // coordonnees barycentriques des sommets, convention p(u, v)= w*a + u*b + v*c, avec w= 1 - u -v
        triangle.ta= vec2(0, 0);        // w= 1
        triangle.tb= vec2(1, 0);        // w= 0
        triangle.tc= vec2(0, 1);        // w= 0
    }
    
    return triangle;
}

void Mesh::bounds( Point& pmin, Point& pmax ) const
{
    if(m_positions.size() < 1)
        return;

    pmin= Point(m_positions[0]);
    pmax= pmin;

    for(unsigned i= 1; i < m_positions.size(); i++)
    {
        vec3 p= m_positions[i];
        pmin= Point( std::min(pmin.x, p.x), std::min(pmin.y, p.y), std::min(pmin.z, p.z) );
        pmax= Point( std::max(pmax.x, p.x), std::max(pmax.y, p.y), std::max(pmax.z, p.z) );
    }
}
