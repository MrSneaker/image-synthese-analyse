
#ifndef _MESH_H
#define _MESH_H

#include <vector>


#include "vec.h"
#include "mat.h"
#include "color.h"
#include "materials.h"

//! \addtogroup objet3D utilitaires pour manipuler des objets 3d
///@{

/*! \file 
representation d'un objet triangule.

Mesh propose plusieurs manieres de decrire un maillage.
par exemple, un triangle :
\code
Mesh m;

// coordonnnees des 3 sommets d'un triangle
Point a= { ... }
Point b= { ... }
Point c= { ... }
m.vertex( a );
m.vertex( b );
m.vertex( c );
\endcode

il est aussi possible de definir d'autres attributs : la couleur du sommet, sa normale, ses coordonnees de texture, ce sont 
les fonctions Mesh::color(), Mesh::normal() et Mesh::texcoord(). on peut decrire ces informations de maniere assez compacte :
\code
Mesh m;

m.color(Red()).vertex(a);
m.color(Green()).vertex(b);
m.color(Blue()).vertex(c);
\endcode
et :
\code
Mesh m;

m.color(Red()).normal(na).vertex(a);
m.color(Green()).normal(nb).vertex(b);
m.color(Blue()).normal(nc).vertex(c);
\endcode

vertex() doit etre utilise en dernier. 
pour permettre de decrire un triangle de couleur uniforme, par exemple, au lieu d'ecrire :
\code
Mesh m;

m.color(Red()).vertex(a);
m.color(Red()).vertex(b);
m.color(Red()).vertex(c);
\endcode
il est plus simple d'ecrire :
\code
Mesh m;

m.color(Red());
m.vertex(a);
m.vertex(b);
m.vertex(c);
\endcode
et cette solution permet aussi de decrire plusieurs triangles partageant leurs sommets, par exemple :
\code
Mesh m;

// insere 4 sommets dans le maillage et conserve leurs indices
unsigned int a= m.vertex(Point(...));
unsigned int b= m.vertex(Point(...));
unsigned int c= m.vertex(Point(...));
unsigned int d= m.vertex(Point(...));

// decrit 2 triangles avec les indices des 4 sommets
m.triangle(a, b, c);
m.triangle(a, c, d);

// ou decrit un triangle indexe sommet par sommet (indice par indice)
m.index(a);
m.index(b);
m.index(c);
\endcode
*/


//! representation d'un triangle.
struct TriangleData
{
    vec3 a, b, c;       //!< positions       
    vec3 na, nb, nc;    //!< normales
    vec2 ta, tb, tc;    //!< texcoords
};

//! representation d'un ensemble de triangles de meme matiere.
struct TriangleGroup
{
    int index;  //!< indice de la "propriete"du groupe de triangles, par defaut : indice de la matiere
    int first;  //!< premier triangle du groupe
    int n;      //!< nombre de triangles du groupe
};


//! representation d'un objet / maillage.
class Mesh
{
public:
    //! \name construction.
    //@{
    //! constructeur par defaut.
    Mesh( ) : m_positions(), m_texcoords(), m_normals(), m_colors(), m_indices(), m_color(White()) {}
    //@}
    
    //! \name description des attributs des sommets. 
    //@{
    //! definit la couleur du prochain sommet.
    Mesh& color( const vec4& c );
    //! definit la couleur du prochain sommet.
    Mesh& color( const Color& c ) { return color(vec4(c.r, c.g, c.b, c.a)); }
    //! definit la couleur du prochain sommet.
    Mesh& color( const float r, const float g, const float b, const float a= 1) { return color(vec4(r, g, b, a)); }
    
    //! definit la normale du prochain sommet.
    Mesh& normal( const vec3& n );
    //! definit la normale du prochain sommet.
    Mesh& normal( const Vector& n ) { return normal(vec3(n)); }
    //! definit la normale du prochain sommet.
    Mesh& normal( const float x, const float y, const float z ) { return normal(vec3(x, y, z)); }
    
    //! definit les coordonnees de texture du prochain sommet.
    Mesh& texcoord( const vec2& uv );
    //! definit les coordonnees de texture du prochain sommet.
    Mesh& texcoord( const float x, const float y ) { return texcoord(vec2(x, y)); }
    
    //! insere un sommet de position p, et ses attributs (s'ils sont definis par color(), texcoord(), normal()), dans l'objet. renvoie l'indice du sommet.
    unsigned vertex( const vec3& p );
    //! insere un sommet de position p, et ses attributs (s'ils sont definis par color(), texcoord(), normal()), dans l'objet. renvoie l'indice du sommet.
    unsigned vertex( const Point& p ) { return vertex(vec3(p)); }
    //! insere un sommet de position p, et ses attributs (s'ils sont definis par color(), texcoord(), normal()), dans l'objet. renvoie l'indice du sommet.
    unsigned vertex( const float x, const float y, const float z ) { return vertex(vec3(x, y, z)); }
    
    //! vide la description.
    void clear( );
    //@}

    //! \name description de triangles indexes.
    //@{
    /*! insere un triangle.  a, b, c sont les indices des sommets deja inseres dans l'objet. ne fonctionne pas avec les strips et les fans.
    \code
    Mesh m;
    unsigned a= m.vertex( ax, ay, az );
    unsigned b= m.vertex( bx, by, bz );
    unsigned c= m.vertex( cx, cy, cz );
    m.triangle(a, b, c);
    \endcode
    */
    Mesh& triangle( const unsigned int a, const unsigned int b, const unsigned int c );
    
    /*! insere un triangle, a, b, c sont les indices des sommets deja inseres dans l'objet, en comptant en partant du dernier. ne fonctionne pas avec les strips et les fans.
    \code
    Mesh m;
    m.vertex( ax, ay, az );
    m.vertex( bx, by, bz );
    m.vertex( cx, cy, cz );
    m.triangle_last(-3, -2, -1);
    \endcode
    */    
    Mesh& triangle_last( const int a, const int b, const int c );
    
    /*! insere un indice de sommet. 
    \code
    Mesh m;
    unsigned a= m.vertex( ax, ay, az );
    unsigned b= m.vertex( bx, by, bz );
    unsigned c= m.vertex( cx, cy, cz );
    
    // insere le triangle abc
    m.index(a);
    m.index(b);
    m.index(c);
    \endcode
    */
    Mesh& index( const int a );
    //@}
    
    //! \name modification des attributs des sommets.
    //@{
    //! modifie la couleur du sommet d'indice id.
    Mesh& color( const unsigned id, const vec4& c );
    //! modifie la couleur du sommet d'indice id.
    Mesh& color( const unsigned id, const Color& c ) { return color(id, vec4(c.r, c.g, c.b, c.a)); }
    //! modifie la couleur du sommet d'indice id.
    Mesh& color( const unsigned id, const float r, const float g, const float b, const float a= 1) { return color(id, vec4(r, g, b, a)); }

    //! modifie la normale du sommet d'indice id.
    Mesh& normal( const unsigned id, const vec3& n );
    //! modifie la normale du sommet d'indice id.
    Mesh& normal( const unsigned id, const Vector& n ) { return normal(id, vec3(n)); }
    //! modifie la normale du sommet d'indice id.
    Mesh& normal( const unsigned id, const float x, const float y, const float z ) { return normal(id, vec3(x, y, z)); }
    
    //! modifie les coordonnees du sommet d'indice id.
    Mesh& texcoord( const unsigned id, const vec2& uv );
    //! modifie les coordonnees du sommet d'indice id.
    Mesh& texcoord( const unsigned id, const float x, const float y ) { return texcoord(id, vec2(x, y)); }
    
    //! modifie la position du sommet d'indice id.
    void vertex( const unsigned id, const vec3& p );
    //! modifie la position du sommet d'indice id.
    void vertex( const unsigned id, const Point& p ) { vertex(id, vec3(p)); }
    //! modifie la position du sommet d'indice id.
    void vertex( const unsigned id, const float x, const float y, const float z ) { vertex(id, vec3(x, y, z)); }
    //@}
    
    //! \name description des matieres.
    //@{
    //! renvoie la description des matieres.
    const Materials& materials( ) const;
    //! renvoie la description des matieres.
    Materials& materials( );
    //! remplace la description des matieres.
    void materials( const Materials& materials );
    
    //! renvoie les indices des matieres des triangles.
    const std::vector<unsigned int>& material_indices( ) const;
    
    //! definit la matiere du prochain triangle. id est l'indice d'une matiere ajoutee dans materials(), cf la classe Materials. ne fonctionne que pour les primitives GL_TRIANGLES, indexees ou pas.
    Mesh& material( const unsigned id );
    //@}
    
    //! \name description des triangles d'un maillage.
    //@{
    //! renvoie le nombre de triangles.
    int triangle_count( ) const;
    //! renvoie un triangle.
    TriangleData triangle( const unsigned id ) const;
    
    //! renvoie l'indice de la matiere d'un triangle.
    int triangle_material_index( const unsigned id ) const;
    //! renvoie la matiere d'un triangle.
    const Material &triangle_material( const unsigned id ) const;
    //@}
    
    //! renvoie min et max les coordonnees des extremites des positions des sommets de l'objet (boite englobante alignee sur les axes, aabb).
    void bounds( Point& pmin, Point& pmax ) const;
    
    //! renvoie la couleur par defaut du mesh, utilisee si les sommets n'ont pas de couleur associee.
    Color default_color( ) const { return m_color; }
    //! modifie la couleur par defaut, utilisee si les sommets n'ont pas de couleur associee.
    Mesh& default_color( const Color& color );
    
    //! \name manipulation des buffers d'attributs.
    //@{
    //! renvoie le nombre de sommets.
    int vertex_count( ) const { return int(m_positions.size()); }
    //! renvoie le nombre d'indices de sommets.
    int index_count( ) const { return int(m_indices.size()); }
    
    const std::vector<vec3>& positions( ) const { return m_positions; }
    const std::vector<vec2>& texcoords( ) const { return m_texcoords; }
    const std::vector<vec3>& normals( ) const { return m_normals; }
    const std::vector<vec4>& colors( ) const { return m_colors; }
    const std::vector<unsigned>& indices( ) const { return m_indices; }
    
    std::vector<vec3>& positions( ) { return m_positions; }
    std::vector<vec2>& texcoords( ) { return m_texcoords; }
    std::vector<vec3>& normals( ) { return m_normals; }
    std::vector<vec4>& colors( ) { return m_colors; }
    std::vector<unsigned>& indices( ) { return m_indices; }
    
    //! verifie que les attributs sont decrits de maniere coherente.
    bool has_position( ) const { return !m_positions.empty(); }
    bool has_texcoord( ) const { return m_texcoords.size() == m_positions.size(); }
    bool has_normal( ) const { return m_normals.size() == m_positions.size(); }
    bool has_color( ) const { return m_colors.size() == m_positions.size(); }
    bool has_material_index( ) const { return int(m_triangle_materials.size()) == triangle_count(); }
    //@}
    
private:
    std::vector<vec3> m_positions;
    std::vector<vec2> m_texcoords;
    std::vector<vec3> m_normals;
    std::vector<vec4> m_colors;
    
    std::vector<unsigned int> m_indices;

    Materials m_materials;
    std::vector<unsigned int> m_triangle_materials;

    Color m_color;
};

///@}

#endif
