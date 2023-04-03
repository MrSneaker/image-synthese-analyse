
#include <cstdlib>
#include <cassert>

#include "mesh_io.h"


bool read_positions( const char *filename, std::vector<Point>& positions )
{
    positions.clear();
    
    FILE *in= fopen(filename, "rt");
    if(in == nullptr)
    {
        printf("[error] loading mesh '%s'...\n", filename);
        return false;
    }
    
    printf("loading mesh '%s'...\n", filename);
    
    std::vector<Point> wpositions;
    std::vector<int> windices;
    
    char line_buffer[1024];
    bool error= true;
    for(;;)
    {
        // charge une ligne du fichier
        if(fgets(line_buffer, sizeof(line_buffer), in) == NULL)
        {
            error= false;       // fin du fichier, pas d'erreur detectee
            break;
        }
        
        // force la fin de la ligne, au cas ou
        line_buffer[sizeof(line_buffer) -1]= 0;
        
        // saute les espaces en debut de ligne
        char *line= line_buffer;
        while(*line && isspace(*line))
            line++;
        
        if(line[0] == 'v')
        {
            float x, y, z;
            if(line[1] == ' ')          // position x y z
            {
                if(sscanf(line, "v %f %f %f", &x, &y, &z) != 3)
                    break;
                wpositions.push_back( Point(x, y, z) );
            }
        }
        
        else if(line[0] == 'f')         // triangle a b c, les sommets sont numerotes a partir de 1 ou de la fin du tableau (< 0)
        {
            windices.clear();
            
            int next= 0;
            for(line= line +1; ; line= line + next)
            {
                windices.push_back(0); 
                
                // analyse les attributs du sommet : p/t/n ou p//n ou p/t ou p...
                next= 0;
                int wt= 0;
                int wn= 0;
                if(sscanf(line, " %d/%d/%d %n", &windices.back(), &wt, &wn, &next) == 3) 
                    continue;
                else if(sscanf(line, " %d/%d %n", &windices.back(), &wt, &next) == 2)
                    continue;
                else if(sscanf(line, " %d//%d %n", &windices.back(), &wn, &next) == 2)
                    continue;
                else if(sscanf(line, " %d %n", &windices.back(), &next) == 1)
                    continue;
                else if(next == 0)      // fin de ligne
                    break;
            }
            
            // triangule la face
            for(unsigned v= 2; v +1 < windices.size(); v++)
            {
                unsigned idv[3]= { 0, v -1, v };
                for(unsigned i= 0; i < 3; i++)
                {
                    unsigned k= idv[i];
                    int p= (windices[k] < 0) ? int(wpositions.size()) + windices[k] : windices[k] -1;
                    if(p < 0) break; // error
                    
                    // et duplique les positions...
                    assert(p < int(wpositions.size()));
                    positions.push_back(wpositions[p]);
                }
            }
        }
    }
    
    fclose(in);
    
    if(error)
        printf("[error] loading mesh '%s'...\n%s\n\n", filename, line_buffer);
    else
        printf("mesh '%s': %d positions\n", filename, int(positions.size()));

    return !error;
}

bool read_indexed_positions( const char *filename, std::vector<Point>& positions, std::vector<int>& indices )
{
    positions.clear();
    indices.clear();
    
    FILE *in= fopen(filename, "rt");
    if(in == nullptr)
    {
        printf("[error] loading mesh '%s'...\n", filename);
        return false;
    }
    
    printf("loading mesh '%s'...\n", filename);
    
    std::vector<int> windices;
    
    char line_buffer[1024];
    bool error= true;
    for(;;)
    {
        // charge une ligne du fichier
        if(fgets(line_buffer, sizeof(line_buffer), in) == NULL)
        {
            error= false;       // fin du fichier, pas d'erreur detectee
            break;
        }
        
        // force la fin de la ligne, au cas ou
        line_buffer[sizeof(line_buffer) -1]= 0;
        
        // saute les espaces en debut de ligne
        char *line= line_buffer;
        while(*line && isspace(*line))
            line++;
        
        if(line[0] == 'v')
        {
            float x, y, z;
            if(line[1] == ' ')          // position x y z
            {
                if(sscanf(line, "v %f %f %f", &x, &y, &z) != 3)
                    break;
                positions.push_back( Point(x, y, z) );
            }
        }
        
        else if(line[0] == 'f')         // triangle a b c, les sommets sont numerotes a partir de 1 ou de la fin du tableau (< 0)
        {
            windices.clear();
            
            int next= 0;
            for(line= line +1; ; line= line + next)
            {
                windices.push_back(0); 
                
                // analyse les attributs du sommet : p/t/n ou p//n ou p/t ou p...
                next= 0;
                int wt= 0;
                int wn= 0;
                if(sscanf(line, " %d/%d/%d %n", &windices.back(), &wt, &wn, &next) == 3) 
                    continue;
                else if(sscanf(line, " %d/%d %n", &windices.back(), &wt, &next) == 2)
                    continue;
                else if(sscanf(line, " %d//%d %n", &windices.back(), &wn, &next) == 2)
                    continue;
                else if(sscanf(line, " %d %n", &windices.back(), &next) == 1)
                    continue;
                else if(next == 0)      // fin de ligne
                    break;
            }
            
            // triangule la face
            for(unsigned v= 2; v +1 < windices.size(); v++)
            {
                unsigned idv[3]= { 0, v -1, v };
                for(unsigned i= 0; i < 3; i++)
                {
                    unsigned k= idv[i];
                    int p= (windices[k] < 0) ? int(positions.size()) + windices[k] : windices[k] -1;
                    if(p < 0) break; // error
                    
                    assert(p < int(positions.size()));
                    indices.push_back(p);
                }
            }
        }
    }
    
    fclose(in);
    
    if(error)
        printf("[error] loading mesh '%s'...\n%s\n\n", filename, line_buffer);
    else
        printf("mesh '%s': %d positions, %d indices\n", filename, int(positions.size()), int(indices.size()));

    return !error;
}
