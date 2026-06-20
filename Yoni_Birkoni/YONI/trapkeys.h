BOOL PeekAndPump();//process messages = Applicatio->ProcessMessages()

void trapsyskeys();//Alt+Esc, Ctrl+Esc, System Key, and Alt+Tab 
				   //are now disabled (trapped)
void untrapsyskeys();//all keys are now enabled

bool ostypent();//return true if windows nt overwise false;