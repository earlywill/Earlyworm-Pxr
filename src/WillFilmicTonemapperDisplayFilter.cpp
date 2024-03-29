/*
# ------------------------------------------------------------------------------
#
# Copyright (c) 1986-2019 Pixar. All rights reserved.
#
# The information in this file (the "Software") is provided for the exclusive
# use of the software licensees of Pixar ("Licensees").  Licensees have the
# right to incorporate the Software into other products for use by other
# authorized software licensees of Pixar, without fee. Except as expressly
# permitted herein, the Software may not be disclosed to third parties, copied
# or duplicated in any form, in whole or in part, without the prior written
# permission of Pixar.
#
# The copyright notices in the Software and this entire statement, including the
# above license grant, this restriction and the following disclaimer, must be
# included in all copies of the Software, in whole or in part, and all permitted
# derivative works of the Software, unless such copies or derivative works are
# solely in the form of machine-executable object code generated by a source
# language processor.
#
# PIXAR DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING ALL
# IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL PIXAR BE
# LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
# WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION
# OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN
# CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.  IN NO CASE WILL
# PIXAR'S TOTAL LIABILITY FOR ALL DAMAGES ARISING OUT OF OR IN CONNECTION WITH
# THE USE OR PERFORMANCE OF THIS SOFTWARE EXCEED $50.
#
# Pixar
# 1200 Park Ave
# Emeryville CA 94608
#
# ------------------------------------------------------------------------------
*/

// Display filter plugin based on Naughty Dog's Filmic 
// Tonemapper from GDC Uncharted 2 HDR Lighting presentation.
//
// See: http://filmicgames.com/archives/75

#include <vector>
#include "RixDisplayFilter.h"
#include <cmath>

void GetFilteredDisplayChannels(
    int const numDisplays,
    RixDisplayChannel const* const displays,
    RtUString const name,
    std::vector<RixDisplayChannel>& ids)
{
    for (int i = 0; i < numDisplays; ++i)
    {
        if (displays[i].channel == name) 
        {
            ids.push_back(displays[i]);
        }
    }
}

// (log10((v + offset) /gain)/ (0.002 / gamma) + whitepoint)/1023
float lin2log(float x, float whitepoint, float gamma, float offset, float gain)
{
    float value = (std::log10( (x + offset) / gain ) / (0.002 / gamma) + whitepoint) / 1023.f;
    return value;
}


class WillFilmicTonemapperDisplayFilter : public RixDisplayFilter
{

public:
    WillFilmicTonemapperDisplayFilter();
    virtual ~WillFilmicTonemapperDisplayFilter();
    virtual int Init(RixContext &ctx, RtUString const pluginPath) override;
    virtual RixSCParamInfo const *GetParamTable() override;
    virtual void Finalize(RixContext &ctx) override;
    virtual void Synchronize(
        RixContext&,
        RixSCSyncMsg,
        RixParameterList const*) override
    {
    }

    virtual void CreateInstanceData(RixContext&, RtUString const, RixParameterList const*,
                                    InstanceData*) override;

    virtual void SynchronizeInstanceData(RixContext&, RtUString const, RixParameterList const*,
                                         uint32_t editHints, InstanceData*) override;

    virtual void Filter(RixDisplayFilterContext &fCtx,
                        RtPointer instanceData) override;

    struct dspyParams
    {
        static void Free(RtPointer dataPtr);
        RtFloat A; 
        RtFloat B; 
        RtFloat C; 
        RtFloat E; 
        RtFloat F; 
        std::vector<RixDisplayChannel> channels;
    };

private:
    static void initializeInstanceData(RixContext& ctx, RtUString const handle,
                                       RixParameterList const* parms, InstanceData* instance);
};

void
WillFilmicTonemapperDisplayFilter::dspyParams::Free(RtPointer dataPtr)
{
    dspyParams *data = static_cast<dspyParams*>(dataPtr);
    delete data;
}

WillFilmicTonemapperDisplayFilter::WillFilmicTonemapperDisplayFilter()
{
}

WillFilmicTonemapperDisplayFilter::~WillFilmicTonemapperDisplayFilter()
{
}

int WillFilmicTonemapperDisplayFilter::Init(RixContext &ctx, RtUString const pluginPath)
{
    PIXAR_ARGUSED(ctx);
    PIXAR_ARGUSED(pluginPath);

    return 0;
}

enum paramIds
{
    k_A,
    k_B,
    k_C,
    k_E,
    k_F,
    k_aov,
    k_numParams
};

RixSCParamInfo const * 
WillFilmicTonemapperDisplayFilter::GetParamTable()
{
    static RixSCParamInfo s_ptable[] =
    {
        RixSCParamInfo(RtUString("A"), k_RixSCFloat),
        RixSCParamInfo(RtUString("B"), k_RixSCFloat),
        RixSCParamInfo(RtUString("C"), k_RixSCFloat),
        RixSCParamInfo(RtUString("E"), k_RixSCFloat),
        RixSCParamInfo(RtUString("F"), k_RixSCFloat),
        RixSCParamInfo(RtUString("aov"), k_RixSCString),
        RixSCParamInfo()
    };
    return &s_ptable[0];
}
    
void WillFilmicTonemapperDisplayFilter::Finalize(RixContext &ctx) 
{
    PIXAR_ARGUSED(ctx);
}

void WillFilmicTonemapperDisplayFilter::CreateInstanceData(RixContext& ctx, RtUString const handle,
                                                          RixParameterList const* parms,
                                                          InstanceData* instance)
{
    PIXAR_ARGUSED(ctx);
    PIXAR_ARGUSED(handle);
    PIXAR_ARGUSED(parms);

    // We want SynchronizeInstanceData() to be called in all cases.
    instance->synchronizeHints = RixShadingPlugin::SynchronizeHints::k_All;
}

void WillFilmicTonemapperDisplayFilter::initializeInstanceData(RixContext& ctx,
                                                              RtUString const handle,
                                                              RixParameterList const* parms,
                                                              InstanceData* instance)
{
    if (instance->data && instance->freefunc)
    {
        (instance->freefunc)(instance->data);
    }

    instance->datalen = sizeof( dspyParams );
    instance->data = new dspyParams();
    instance->freefunc = dspyParams::Free;
    dspyParams *p = reinterpret_cast< dspyParams * >( instance->data );

    p->A = 95.f; // blackpoint
    p->B = 685.f; // whitepoint
    p->C = 0.6f; // gamma
    p->E = 1.f; // offset
    p->F = 1.f; // gain
    RtUString aov("Ci");

    parms->EvalParam( k_A, 0, &p->A);
    parms->EvalParam( k_B, 0, &p->B);
    parms->EvalParam( k_C, 0, &p->C);
    parms->EvalParam( k_aov, 0, &aov);
    
    p->E = std::pow(10,(p->A - p->B) * 0.002 / p->C);
    p->F = 1.f/(1.f-p->E);

    RixMessages *msg = reinterpret_cast< RixMessages * >(
        ctx.GetRixInterface( k_RixMessages ) );
    RixRenderState *renderState = static_cast<RixRenderState*>(
        ctx.GetRixInterface(k_RixRenderState));
    RixRenderState::FrameInfo frameInfo;
    renderState->GetFrameInfo(&frameInfo);
    RixIntegratorEnvironment const *integratorEnv = frameInfo.integratorEnv;
    if (!integratorEnv) return;

    p->channels.clear();
    GetFilteredDisplayChannels(
        integratorEnv->numDisplays,
        integratorEnv->displays,
        aov,
        p->channels);
    
    if (p->channels.empty())
    {
        msg->Warning( "WillFilmicTonemapperDisplayFilter '%s' did not find a "
                      "display for AOV '%s'", handle.CStr(), aov.CStr() );
        return;
    }

    return;
}

void WillFilmicTonemapperDisplayFilter::SynchronizeInstanceData(
    RixContext& rixCtx, RtUString const handle, RixParameterList const* instanceParams,
    uint32_t editHints, InstanceData* instanceData)
{
    PIXAR_ARGUSED(editHints);
    assert(instanceData);

    initializeInstanceData(rixCtx, handle, instanceParams, instanceData);

    return;
}

void WillFilmicTonemapperDisplayFilter::Filter(RixDisplayFilterContext &fCtx,
                                              RtPointer instanceData)
{
    // we didn't find the proper aov, so we do not need to do anything
    if(!instanceData)
    {
        return;
    }

    dspyParams const* p = reinterpret_cast< dspyParams const* >(instanceData);
    
    for (int d = 0; d < p->channels.size(); ++d)
    {
        for (int x = fCtx.xmin; x < fCtx.xmax; ++x) {
            for (int y= fCtx.ymin; y < fCtx.ymax; ++y) {
                RtColorRGB pixel;
                fCtx.Read(p->channels[d].id, x, y, pixel);

                pixel[0] = lin2log(pixel[0], p->B, p->C, p->E, p->F);
                pixel[1] = lin2log(pixel[1], p->B, p->C, p->E, p->F);
                pixel[2] = lin2log(pixel[2], p->B, p->C, p->E, p->F);

                fCtx.Write(p->channels[d].id, x, y, pixel);
            }        
        }
    }
}

RIX_DISPLAYFILTERCREATE
{
    PIXAR_ARGUSED(hint);
    return new WillFilmicTonemapperDisplayFilter();
}

RIX_DISPLAYFILTERDESTROY
{
    delete reinterpret_cast< WillFilmicTonemapperDisplayFilter * >( filter );
}
