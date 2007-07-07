#include "xerxes.h"
#include "g_script.h"

ScriptEngine *se;

void HookTimer()
{
	if (strlen(timerfunc))
	{
		while (hooktimer)
		{
			se->ExecuteFunctionString(timerfunc);
			hooktimer--;
		}
	}
}

void HookRetrace()
{
	if (strlen(renderfunc))
		se->ExecuteFunctionString(renderfunc);
}

image *ImageForHandle(int handle)
{
	if (handle == 0)
		vc->vcerr("ImageForHandle() - Null image reference, probably an uninitialized image handle");

	if (handle<0 || handle >= Handle::getHandleCount(HANDLE_TYPE_IMAGE) )
		vc->vcerr("ImageForHandle() - Image reference is bogus! (%d)", handle);

	image* ptr = (image*) Handle::getPointer(HANDLE_TYPE_IMAGE,handle);

	if (ptr == NULL)
		vc->vcerr("ImageForHandle() - Image reference is valid but no image is allocated for this handle. You may have mistakenly freed it and continued to use it.");
	return ptr;
}


void FreeImageHandle(int handle)
{
	Handle::free(HANDLE_TYPE_IMAGE,handle);
}

void SetHandleImage(int handle, image *img)
{
	Handle::setPointer(HANDLE_TYPE_IMAGE, handle, (void*)img);
}

int HandleForImage(image *img)
{
	return Handle::alloc(HANDLE_TYPE_IMAGE, img);
}