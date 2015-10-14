#include "ClassDemoApp.h"

int main(int argc, char *argv[])
{
	ClassDemoApp app = ClassDemoApp();

	while (!app.isDone()) {
		app.UpdateAndRender();
	}

	return 0;
}
