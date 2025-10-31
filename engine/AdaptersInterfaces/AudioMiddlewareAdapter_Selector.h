#ifndef __AUDIOMIDDLEWAREADAPTER_SELECTOR_H__
#define __AUDIOMIDDLEWAREADAPTER_SELECTOR_H__




// --- --- --- --- AUDIO ENGINE CHOICE --- --- --- ---
#if defined(ITF_PS5) || defined(ITF_WINDOWS) || defined(ITF_NX) || defined(ITF_OUNCE)


#if defined(ITF_WINDOWS) && defined(ITF_RETAIL) 
#define ITF_WWISE_PROFILE
#endif

#define ITF_SUPPORT_WWISE

#else
#define ITF_SUPPORT_RAKI
#endif


#if defined(ITF_SUPPORT_WWISE)
#if defined(ITF_SUPPORT_RAKI) 
#error "(macros.h) : Choose only one audio engine"
#endif
#endif



// --- --- --- --- AUDIO ENGINE CHOICE --- --- --- ---


#endif // __AUDIOMIDDLEWAREADAPTER_SELECTOR_H__
