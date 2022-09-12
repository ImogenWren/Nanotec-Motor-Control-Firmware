/*
This NanoJ Example Code is based on our experience with typical user requirements in a wide range
of industrial applications and is provided without guarantee regarding correctness and completeness.
It serves as general guidance and should not be construed as a commitment of Nanotec to guarantee its
applicability to the customer application without additional tests under the specific conditions
and – if and when necessary – modifications by the customer. 

The responsibility for the applicability and use of the NanoJ Example Code in a particular
customer application lies solely within the authority of the customer.
It is the customer's responsibility to evaluate, investigate and decide,
whether the Example Code is valid and suitable for the respective customer application, or not.
Defects resulting from the improper handling of devices and modules are excluded from the warranty.
Under no circumstances will Nanotec be liable for any direct, indirect, incidental or consequential damages
arising in connection with the Example Code provided. In addition, the regulations regarding the
liability from our Terms and Conditions of Sale and Delivery shall apply.
*/

#ifndef NANOTEC_H_
#define NANOTEC_H_

void Reset()
{
	InOut.Controlword = InOut.Controlword | 0x80;
	yield(); 
}

void Quickstop()
{
	while ((In.Statusword & 0x20) != 0x00) 
	{
		InOut.Controlword = InOut.Controlword& 0xFF70;
		yield(); 
	}	
	yield();
}

void Shutdown()
{
	while ((In.Statusword & 0xEF) != 0x21) 
	{
		InOut.Controlword = (InOut.Controlword& 0xFF76)|0x0006;
		yield(); 
	}
	yield();
}

void SwitchOn()
{
	while ((In.Statusword & 0xE9) != 0x21) 
	{
		InOut.Controlword = (InOut.Controlword& 0xFF76)|0x0006;
		yield(); 
	}
	while ((In.Statusword & 0xEF) != 0x23) 
	{
		InOut.Controlword = (InOut.Controlword& 0xFF77)|0x0007;
		yield(); 
	}
	yield();
}

void EnableOperation()
{
	while ((In.Statusword & 0xE9) != 0x21) 
	{
		InOut.Controlword = (InOut.Controlword& 0xFF76)|0x0006;
		yield(); 
	}
	while ((In.Statusword & 0xEB) != 0x23) 
	{
		InOut.Controlword = (InOut.Controlword& 0xFF77)|0x0007;
		yield(); 
	}
	while ((In.Statusword & 0xEF) != 0x27) 
	{
		InOut.Controlword = (InOut.Controlword& 0xFF7F)|0x000F;							
		yield(); 
	}	
	yield();
}	

void RelativeMovement()
{	
	InOut.Controlword = InOut.Controlword | 0x0040;
	yield();
}

void AbsoluteMovement()
{
	InOut.Controlword = InOut.Controlword & 0xFFBF;
	yield();
}

void NewSetPoint(bool newsetpoint)
{
	if(newsetpoint)
	{	
		InOut.Controlword = InOut.Controlword | 0x0010;
		yield();
	}
	else
	{
		InOut.Controlword = InOut.Controlword & 0xFFEF;
		yield();
	}
}

void Halt(bool halt)
{
	if(halt)
	{
		InOut.Controlword = InOut.Controlword | 0x0100;
		yield();
	}
	else
	{
		InOut.Controlword = InOut.Controlword & 0xFEFF;
		yield();
	}	
}

void ChangeSetPointImmediately(bool changesetpointimmediately)
{
	if(changesetpointimmediately)
	{
		InOut.Controlword = InOut.Controlword | 0x0020;
		yield();
	}
	else
	{
		InOut.Controlword = InOut.Controlword & 0xFFDF;
		yield();
	}	
}

void ChangeOnSetPoint(bool changeonsetpoint)
{
	if(changeonsetpoint)
	{
		InOut.Controlword = InOut.Controlword | 0x0200;
		yield();
	}
	else
	{
		InOut.Controlword = InOut.Controlword & 0xFDFF;
		yield();
	}	
}

bool TargetReached()
{
	if((In.Statusword & 0x400) == 0x400)
	{
		return true;
	}
	else
	{
		return false;
	}	
}		

bool NewSetPointAcknowledge()
{
	if((In.Statusword & 0x1000) == 0x1000)
	{
		return true;
	}
	else
	{
		return false;
	}	
}	

bool DigitalInput(U08 inputs)
{
	if(In.Inputs & (1 << (15+inputs)))
	{
		return true;
	}
	else
	{
		return false;	
	}		
}

void SetDigitalOutput(U08 outputs)
{
	U32 OutputBit = 1 << (15+outputs);
	InOut.Outputs = InOut.Outputs | OutputBit;	
}

void ClearDigitalOutput(U08 outputs)
{
	U32 OutputBit = 1 << (15+outputs);
	InOut.Outputs = InOut.Outputs & ~OutputBit;		
}

void ModesOfOperation(S08 Mode)
{
	Out.ModesOfOperation = Mode;
}	

S08 ModesOfOperationDisplay()
{
	return In.ModesOfOperationDisplay;
}	

S16 AnalogInput()
{
	return In.AnalogInput;
}	


 #endif /*NANOTEC_H_*/