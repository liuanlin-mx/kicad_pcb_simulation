#ifndef __Z0_CALC_H__
#define __Z0_CALC_H__

#include <memory>

class Z0_calc
{
public:
    enum
    {
        Z0_CALC_ATLC = 0,
        Z0_CALC_MMTL,
    };
    
public:
    Z0_calc() {};
    virtual ~Z0_calc() {}
    
public:
    virtual void set_tmp_name(const std::string& tmp_name) {}
    
    /* 设置物体最小尺寸 */
    virtual void set_precision(float unit = 0.035) {}
    
    virtual void set_box_size(float w, float h) {}
    virtual std::uint32_t get_type() = 0;
    
    virtual void clean() {}
    virtual void clean_all() {}
    
    /* 坐标是盒子的中心点 */
    virtual void add_ground(float x, float y, float w, float thickness) {}
    virtual void add_wire(float x, float y, float w, float thickness) {}
    virtual void add_coupler(float x, float y, float w, float thickness) {}
    virtual void add_elec(float x, float y, float w, float thickness, float er = 4.6) {}
    virtual bool calc_zo(float& Zo, float& v, float& c, float& l, float& r, float& g) = 0;
    virtual bool calc_coupled_zo(float& Zodd, float& Zeven, float& Zdiff, float& Zcomm,
                        float& Lodd, float& Leven, float& Codd, float& Ceven) { return true; }
                        
public:
    static std::shared_ptr<Z0_calc> create(std::uint32_t type);
};

#endif