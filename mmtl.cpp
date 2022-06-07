#include <string.h>
#include "mmtl.h"

static const char *base_xsctn = "package require csdl\n\n"
                "set _title \"Example Coplanar Waveguide\"\n"
                "set ::Stackup::couplingLength \"0.0254\"\n"
                "set ::Stackup::riseTime \"25\"\n"
                "set ::Stackup::frequency \"1000MHz\"\n"
                "set ::Stackup::defaultLengthUnits \"meters\"\n"
                "set CSEG 10\n"
                "set DSEG 10\n"
                
                "GroundPlane ground  \\\n"
                "   -thickness 0.035 \\\n"
                "   -yOffset 0.0 \\\n"
                "   -xOffset 0.0\n"
                "DielectricLayer dielAir  \\\n"
                "   -thickness 5 \\\n"
                "   -lossTangent 0.0002 \\\n"
                "   -permittivity 1.0 \\\n"
                "   -permeability 1.0 \\\n"
                "   -yOffset 0.0 \\\n"
                "   -xOffset 0.0\n";
                
mmtl::mmtl()
{
    _tmp_name = "tmp";
    _xsctn = base_xsctn;
    _cond_id = 0;
    _gnd_id = 0;
    _elec_id = 0;
}


mmtl::~mmtl()
{
}

void mmtl::set_precision(float unit)
{
}


void mmtl::set_box_size(float w, float h)
{
}


void mmtl::clean()
{
    _map.clear();
    _xsctn = base_xsctn;
    _cond_id = 0;
    _gnd_id = 0;
    _elec_id = 0;
}


void mmtl::clean_all()
{
    _map.clear();
    _xsctn = base_xsctn;
    _cond_id = 0;
    _gnd_id = 0;
    _elec_id = 0;
}


void mmtl::add_ground(float x, float y, float w, float thickness)
{
    item item_;
    item_.type = ITEM_TYPE_GND;
    item_.x = x;
    item_.y = y;
    item_.w = w;
    item_.h = thickness;
    
    _map.emplace(y, item_);
}


void mmtl::add_wire(float x, float y, float w, float thickness)
{
    item item_;
    item_.type = ITEM_TYPE_COND;
    item_.x = x;
    item_.y = y;
    item_.w = w;
    item_.h = thickness;
    _map.emplace(y, item_);
}


void mmtl::add_coupler(float x, float y, float w, float thickness)
{
    add_wire(x, y, w, thickness);
}


void mmtl::add_elec(float x, float y, float w, float thickness, float er)
{
    item item_;
    item_.type = ITEM_TYPE_ELEC;
    item_.x = x;
    item_.y = y;
    item_.w = w;
    item_.h = thickness;
    item_.er = er;
    _map.emplace(y, item_);
}


bool mmtl::calc_zo(float & Z0, float & v, float & c, float & l, float& r, float& g)
{
    char cmd[512] = {0};
    char buf[1024] = {0};
    
    _build();
    sprintf(buf, "%s.xsctn", _tmp_name.c_str());
    FILE *fp = fopen(buf, "wb");
    if (fp)
    {
        fwrite(_xsctn.c_str(), 1, _xsctn.length(), fp);
        fflush(fp);
        fclose(fp);
    }
    
    
    sprintf(cmd, "mmtl_bem %s", _tmp_name.c_str());
    
    FILE *pfp = popen(cmd, "r");
    while (fgets(buf, sizeof(buf), pfp))
    {
        //printf("%s", buf);
    }
    pclose(pfp);
    _read_value(Z0, v, c, l, r, g);
    //printf("Zo:%f v:%fmm/ns c:%f l:%f\n", Z0, v / 1000000, c, l);
    return false;
}

bool mmtl::calc_coupled_zo(float & Zodd, float & Zeven, float & Zdiff, float & Zcomm, float & Lodd, float & Leven, float & Codd, float & Ceven)
{
    return false;
}




void mmtl::_add_ground(float x, float y, float w, float thickness)
{
    char buf[128];
    sprintf(buf, "RectangleConductors groundWires%d  \\\n", _gnd_id++);
    _xsctn += buf;
    
    sprintf(buf, "-width %f \\\n", w);
    _xsctn += buf;
    _xsctn += "-pitch 1 \\\n";
    _xsctn += "-conductivity 3e+07siemens/meter \\\n";
    
    sprintf(buf, "-height %f \\\n", thickness);
    _xsctn += buf;
    
    _xsctn += "-number 1 \\\n";
    
    sprintf(buf, "-yOffset %f \\\n", y);
    _xsctn += buf;
    
    sprintf(buf, "-xOffset %f \n", x - w * 0.5);
    _xsctn += buf;
}

void mmtl::_add_wire(float x, float y, float w, float thickness)
{
    char buf[128];
    
    sprintf(buf, "RectangleConductors cond%d  \\\n", _cond_id++);
    _xsctn += buf;
    
    sprintf(buf, "-width %f \\\n", w);
    _xsctn += buf;
    _xsctn += "-pitch 1 \\\n";
    _xsctn += "-conductivity 3e+07siemens/meter \\\n";
    
    sprintf(buf, "-height %f \\\n", thickness);
    _xsctn += buf;
    
    _xsctn += "-number 1 \\\n";
    
    sprintf(buf, "-yOffset %f \\\n", y);
    _xsctn += buf;
    
    sprintf(buf, "-xOffset %f \n", x - w * 0.5);
    _xsctn += buf;
}


void mmtl::_add_elec(float x, float y, float w, float thickness, float er)
{
    char buf[128];
    
    sprintf(buf, "DielectricLayer DielLaye%d  \\\n", _elec_id++);
    _xsctn += buf;
    
    sprintf(buf, "-thickness %f \\\n", thickness);
    _xsctn += buf;
    
    _xsctn += "-lossTangent 0.0002 \\\n";
    
    sprintf(buf, "-permittivity %f \\\n", er);
    _xsctn += buf;
    
    _xsctn += "-permeability 1.0 \\\n";
    
     
    sprintf(buf, "-yOffset %f \\\n", y);
    _xsctn += buf;
    
    sprintf(buf, "-xOffset %f \n", x - w * 0.5);
    _xsctn += buf;
}


void mmtl::_build()
{
    _xsctn = base_xsctn;
    float y = 0;
    float h = 0;
    bool flag = false;
    for (const auto& it: _map)
    {
        const item& item_ = it.second;
        if (flag && y != item_.y)
        {
            flag = false;
            _add_elec(0, 0, 0, h, 1.);
        }
        
        y = item_.y;
        
        if (item_.type == ITEM_TYPE_ELEC)
        {
            _add_elec(item_.x, 0, item_.w, item_.h, item_.er);
        }
        else if (item_.type == ITEM_TYPE_GND)
        {
            _add_ground(item_.x, 0, item_.w, item_.h);
            //_add_elec(item_.x, 0, item_.w, item_.h, 1.);
            flag = true;
            h = item_.h;
        }
        else if (item_.type == ITEM_TYPE_COND)
        {
            _add_wire(item_.x, 0, item_.w, item_.h);
            //_add_elec(item_.x, 0, item_.w, item_.h, 1.);
            flag = true;
            h = item_.h;
        }
    }
}


void mmtl::_read_value(float & Z0, float & v, float & c, float & l, float& r, float& g)
{
    char buf[4096];
    
    sprintf(buf, "%s.result", _tmp_name.c_str());
    FILE *fp = fopen(buf, "rb");
    if (fp == NULL)
    {
        return;
    }
    
    
    int rlen = fread(buf, 1, sizeof(buf) - 1, fp);
    buf[rlen] = 0;
    fclose(fp);
    
    char *s = strstr(buf, "B( ::cond0R0 , ::cond0R0 )=   ");
    if (s)
    {
        s += strlen("B( ::cond0R0 , ::cond0R0 )=   ");
        c = atof(s) * 1e12;
    }
    
    s = strstr(buf, "L( ::cond0R0 , ::cond0R0 )=   ");
    if (s)
    {
        s += strlen("L( ::cond0R0 , ::cond0R0 )=   ");
        l = atof(s) * 1e9;
    }
    
    s = strstr(buf, "Characteristic Impedance (Ohms):");
    if (s)
    {
        s = strstr(s, "::cond0R0= ");
        if (s)
        {
            s += strlen("::cond0R0= ");
            Z0 = atof(s);
        }
    }
    
    s = strstr(buf, "Propagation Velocity (meters/second):");
    if (s)
    {
        s = strstr(s, "::cond0R0=   ");
        if (s)
        {
            s += strlen("::cond0R0=   ");
            v = atof(s);
        }
    }
    
    s = strstr(buf, "Rdc( ::cond0R0 , ::cond0R0 )=  ");
    if (s)
    {
        s += strlen("Rdc( ::cond0R0 , ::cond0R0 )=  ");
        r = atof(s);
    }
    g = 0;
}