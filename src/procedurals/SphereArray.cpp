
#include "ri.h"
#include "prmanapi.h"
#include "RixPredefinedStrings.hpp"
#include <string.h>
#include <stdlib.h>
#include <random>
#include <iostream>

#include <time.h>

std:: default_random_engine generator;
std::uniform_real_distribution<double> distr(0.0,1.0);
double erand48(int X){
    return distr(generator);
}

// Function to print the current time
void print_current_time(const char* label) {
    // Print a label to indicate where the time check is occurring
    printf("%s\n", label); 
    
    // Get current time
    time_t t = time(NULL); 
    
    // Convert to local time structure
    struct tm tm = *localtime(&t); 
    
    // Print the formatted time
    printf("now: %d-%02d-%02d %02d:%02d:%02d\n", 
           tm.tm_year + 1900, 
           tm.tm_mon + 1, 
           tm.tm_mday, 
           tm.tm_hour, 
           tm.tm_min, 
           tm.tm_sec);
}


//double erand48(int X)
//{
//    srand(X);
//    return (double)rand() / (double)RAND_MAX;
//}

struct Params
{
    Params(RtInt n, RtToken const toks[], RtPointer const vals[]);
    // radius of sphere
    float radius;
    // number of spheres
    int total;
    // spread of sphere
    float spread;
    float displacementbound;
    RtUString bxdf;
    RtUString light;
    RtUString displace;
};

Params::Params(RtInt n, RtToken const toks[], RtPointer const vals[]) :
    radius(1),
    total(1),
    spread(1),
    displacementbound(0),
    bxdf(US_NULL),
    light(US_NULL),
    displace(US_NULL)
{
    while (--n >= 0)
    {
        if (std::strstr(toks[n], "radius"))
        {
            radius = *(float*)vals[n];
        }
        else if (std::strstr(toks[n], "total"))
        {
            total = *(int*)vals[n];
        }
        else if (std::strstr(toks[n], "spread"))
        {
            spread = *(float*)vals[n];
        }
        else if (std::strstr(toks[n], "displacementbound"))
        {
            displacementbound = *(float*)vals[n];
        }
        else if (std::strstr(toks[n], "bxdf"))
        {
            bxdf = *(RtUString*)vals[n];
        }
        else if (std::strstr(toks[n], "light"))
        {
            light = *(RtUString*)vals[n];
        }
        else if (std::strstr(toks[n], "displace"))
        {
            displace = *(RtUString*)vals[n];
        }
    }
}

extern "C" PRMANEXPORT RtVoid
Bound(RtInt n, RtToken const toks[], RtPointer const vals[], RtBound result[2])
{
    Params const params(n, toks, vals);
    float const radius = params.radius + params.displacementbound;
    result[0][0] = result[1][0] = -radius;
    result[0][1] = result[1][1] =  radius;
    result[0][2] = result[1][2] = -radius;
    result[0][3] = result[1][3] =  radius;
    result[0][4] = result[1][4] = -radius;
    result[0][5] = result[1][5] =  radius;
}

extern "C" PRMANEXPORT RtVoid
Subdivide2(RtContextHandle ctx, RtFloat detail,
          RtInt n, RtToken const toks[], RtPointer const vals[])
{
    // The Bxdf/Displace "__materialid" corresponds to the shading group name in RfM
    static RtToken const k_type = Rix::k_empty.CStr(); // Should be empty
    static RtToken const k_handle = Rix::k_empty.CStr();
    static RtToken const k_materialId = RtUString("string __materialid").CStr();
    static RtToken const k_displacementbound = Rix::k_displacementbound.CStr();
    static RtToken const k_sphere = RtUString("float sphere").CStr();
    Params const params(n, toks, vals);
    if (!params.bxdf.Empty())
    {
        RiBxdf(k_type, k_handle, k_materialId, &params.bxdf, RI_NULL);
    }
    if (!params.light.Empty())
    {
        RiLight(k_type, k_handle, k_materialId, &params.light, RI_NULL);
    }
    if (!params.displace.Empty())
    {
        RiAttribute(k_displacementbound, k_sphere, &params.displacementbound, RI_NULL);
        RiDisplace(k_type, k_handle, k_materialId, &params.displace, RI_NULL);
    }
    
    char numstr[64]; // enough to hold all numbers up to 64-bits
    
    // define primary sphere
    RtObjectHandle pHandle;
    pHandle = RiObjectBegin();
    RtInt sphereid = 1000;
    RiAttribute("identifier", "int id", &sphereid, RI_NULL);
    sphereid += 1;
    RiSphere(params.radius, -params.radius, params.radius, 360.0f, RI_NULL);
    RiObjectEnd();
    
	print_current_time("Starting execution...");
        
    for (int x = 0; x < params.total; x = x + 1)
    {
        RiAttributeBegin();
        
        // id each sphere
        //sprintf(numstr, "sph_%d", x);
        //RtToken spherename = numstr;
        //sphereid += x;
        //RiAttribute("identifier", "string name", &spherename, RI_NULL);
        //RiAttribute("identifier", "int id", &sphereid, RI_NULL);
        //Attribute "identifier" "int id2" [835737221]
        
        
        //double Xi = erand48(x);
        //std::cout << Xi << std::endl;
        // transform the sphere
        RiTranslate((erand48(x+12)-0.5)*params.spread, (erand48(x+17)-0.5)*params.spread, (erand48(x+23)-0.5)*params.spread);
        
        // instance sphere
        RiObjectInstance (pHandle);
        
        // create sphere
        //RiSphere(params.radius, -params.radius, params.radius, 360.0f, RI_NULL);
        
        RiAttributeEnd();
        
    }
    print_current_time("all done...");
    
    
}

