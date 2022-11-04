/*************************************************************************************************
 * @file		atomic.h
 *
 * @brief		Interrupt blocking macros WARNING!!! ONLY STM32
 *
 * @version		v1.0
 * @date		30.11.2013
 * @author		Mike Smith
 * @CodeReview	Shpegun60
 ************************************************************************************************/
#ifndef ENTITY_ATOMIC_H
#define ENTITY_ATOMIC_H

static __inline __attribute__((always_inline)) int __irqDis(int flag)
{
	if (flag)
	{
		__asm volatile ("cpsid i" : : : "memory");
	}
    return 1;
}

static __inline __attribute__((always_inline)) int __irqEn(int flag)
{
	if (flag)
	{
		__asm volatile ("cpsie i" : : : "memory");
	}
	return 0;
}


#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wmaybe-uninitialized"

static __inline __attribute__((always_inline)) int _irqDisGetPrimask(int flag)
{
	int result;
	if (flag)
	{
		__asm volatile ("MRS %0, primask" : "=r" (result) );
		__asm volatile ("cpsid i" : : : "memory");
	}
	return result;
}

static __inline __attribute__((always_inline)) int _irqSetPrimask(int priMask)
{
	__asm volatile ("MSR primask, %0" : : "r" (priMask) : "memory");
	return 0;
}

#pragma GCC diagnostic pop


// block with a preliminary check of the need to prohibit and subsequent unconditional permission of interrupts
// if the condition is not met, the interrupt enable flag will not be changed
#define ATOMIC_BLOCK_FORCEON_COND(condition) \
		for(int cond = condition, flag = __irqDis(cond);\
		flag;\
		flag = __irqEn(cond))\

// block with a preliminary check of the need to prohibit
// if the condition is not met, the interrupt enable flag will not be changed
#define ATOMIC_BLOCK_RESTORATE_COND(condition) \
		for(int cond = condition, mask = _irqDisGetPrimask(cond), flag = 1;\
		flag;\
		flag = cond ? _irqSetPrimask(mask):0)

// block with prohibition and subsequent unconditional interrupt enable
#define ATOMIC_BLOCK_FORCEON  	ATOMIC_BLOCK_FORCEON_COND(true)

// block with prohibition and subsequent restoration of interrupts
#define ATOMIC_BLOCK_RESTORATE	ATOMIC_BLOCK_RESTORATE_COND(true)



#endif
