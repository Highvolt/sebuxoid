#include "ints.h"
#include <stdio.h>

/** Installs the ASM written function 'irq_func' as the interrupt
 * handler associated with interrupt 'irq', updating the 'old_irq'
 * structure with the current interrupt handler.  Returns 0 on success
 * or -1 on error, in which case the original interrupt handler
 * remains installed.
 * 
 * See reinstall_asm_irq_handler() to reinstall the original handler.
 */
int install_asm_irq_handler(int irq, void (*irq_func)(void),
				_go32_dpmi_seginfo *old_irq)
{
	//devolve o vector correspondente ao IRQ pretendido
	int vector = IRQ_VECTOR(irq);
	
	//obter endereco do handler original
	if(_go32_dpmi_get_protected_mode_interrupt_vector(vector, old_irq)) return -1;
	
	//variavel para o endereco do novo handler
	_go32_dpmi_seginfo new_irq;
	new_irq.pm_selector = _go32_my_cs(); // selector do codigo
	new_irq.pm_offset = (unsigned long)irq_func; // endereco do novo handler
	
	//instalacao do handler na IDT
	if(_go32_dpmi_set_protected_mode_interrupt_vector(vector, &new_irq)) return -1;
	return 0;
}


/** Installs the C written 'irq_func' interrupt handler at the
 * interrupt handler associated with interrupt 'irq', updating the
 * old_irq structure with the current interrupt handler.  Returns 0 on
 * success or -1 on error, in which case the original interrupt
 * handler remains installed.
 * 
 * See reinstall_c_irq_handler() to reinstall the original handler.
 */
int install_c_irq_handler(int irq, void (*irq_func)(void),
			    _go32_dpmi_seginfo *old_irq)
{
	//devolve o vector correspondente ao IRQ pretendido
	int vector = IRQ_VECTOR(irq);
	
	//endereco do handler original
	if(_go32_dpmi_get_protected_mode_interrupt_vector(vector, old_irq)) return -1;
	
	//variavel para o endereco do handler
	_go32_dpmi_seginfo new_irq;
	new_irq.pm_selector = _go32_my_cs(); // selector do codigo
	new_irq.pm_offset = (unsigned long)irq_func; // endereco do novo handler
	
	//prepara função assembly para chamar função C
	if(_go32_dpmi_allocate_iret_wrapper(&new_irq)) return -1;
	
	// instalacao do handler na IDT
	if(_go32_dpmi_set_protected_mode_interrupt_vector(vector, &new_irq)) return -1;
	return 0;
}


/** Reinstalls the interrupt handler specified in 'old_irq' for interrupt
 * 'irq'. See install_asm_irq_handler().
 */
void reinstall_asm_irq_handler(int irq, _go32_dpmi_seginfo *old_irq)
{
	//devolve o vector correspondente ao IRQ pretendido
	int vector = IRQ_VECTOR(irq);
	
	//repoe o handler original
	_go32_dpmi_set_protected_mode_interrupt_vector(vector, old_irq);
}


/** Reinstalls the interrupt handler specified in 'old_irq' for interrupt
 * 'irq'. See install_c_irq_handler().
 */
void reinstall_c_irq_handler(int irq, _go32_dpmi_seginfo *old_irq)
{
	//devolve o vector correspondente ao IRQ pretendido
	int vector = IRQ_VECTOR(irq);
	
	//guarda apontador para o handler actual
	_go32_dpmi_seginfo new_irq;
	_go32_dpmi_get_protected_mode_interrupt_vector(vector, &new_irq);
	
	//repoe o handler original
	_go32_dpmi_set_protected_mode_interrupt_vector(vector, old_irq);
	
	//liberta função assembly
	_go32_dpmi_free_iret_wrapper(&new_irq);
}

/*
void printf_bin(Byte b)
{
	int offset;
	for(offset = 7; offset >= 0; offset--)
		printf("%i", (b >> offset) & 1);
}*/

/** Mask the bit corresponding to 'irq', disabling its interrupts.
 * The correct PIC is used.
 */
/*void mask_pic(int irq)
{
	Word pic_port = (irq < 8 ? PIC1_MASK : PIC2_MASK);
	Byte val = inportb(pic_port) | IRQ_MASK_BIT(irq);
	//printf("Mask: port %X <- ", pic_port); printf_bin(val); printf("\n");
	outportb(pic_port, val);
}*/


/** Unmask the bit corresponding to 'irq', enabling its interrupts.
 * The correct PIC is used.
*//*
void unmask_pic(int irq)
{
	Word pic_port = (irq < 8 ? PIC1_MASK : PIC2_MASK);
	Byte val = inportb(pic_port) & ~IRQ_MASK_BIT(irq);
	//printf("Unmask: port %X <- ", pic_port); printf_bin(val); printf("\n");
	outportb(pic_port, val);
}*/

void mask_pic(int irq)
{
	unsigned short pic;
	
	if (irq < 8)
		pic = PIC1_MASK;
	else
		pic = PIC2_MASK;
	
	unsigned char mascara = inportb(pic) | IRQ_MASK_BIT(irq); 
	outportb(pic, mascara); 
}

void unmask_pic(int irq)
{
	unsigned short pic;
	
	if (irq <= 7) 
		pic = PIC1_MASK;
	else
		pic = PIC2_MASK;
		
	unsigned char mascara = inportb(pic) & (~IRQ_MASK_BIT(irq));
	outportb(pic, mascara); 
}


void set_ints(Bool on)
{
	static Byte state_pic1;
	static Byte state_pic2;
	
	if(on)
	{
		outportb(PIC1_MASK, state_pic1);
		outportb(PIC2_MASK, state_pic2);
	}
	else
	{
		state_pic1 = inportb(PIC1_MASK);
		state_pic2 = inportb(PIC2_MASK);
		outportb(PIC1_MASK, 0xFF);
		outportb(PIC2_MASK, 0xFF);
	}
}


Byte get_pic1() {
	return inportb(PIC1_MASK);
}


Byte get_pic2() {
	return inportb(PIC2_MASK);
}


void set_pic_var(Byte pic1, Byte pic2)
{
	outportb(PIC1_MASK, pic1);
	outportb(PIC2_MASK, pic2);
}


void mask_pic_var(int irq, Byte* pic1, Byte* pic2)
{
	Byte* pic;
	
	if (irq < 8) pic = pic1;
	else pic = pic2;
	
	(*pic) |= IRQ_MASK_BIT(irq);
}


void unmask_pic_var(int irq, Byte* pic1, Byte* pic2)
{
	Byte* pic;
	
	if (irq < 8) pic = pic1;
	else pic = pic2;
	
	(*pic) &= (~IRQ_MASK_BIT(irq));
}

