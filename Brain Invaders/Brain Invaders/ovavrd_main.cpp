#include "ovavrdCOgreVRApplication.h"
#include "SpaceInvadersBCI/ovavrdCSpaceInvadersBCI.h"

#if 0
#if defined OVA_OS_Linux
namespace CEGUI
{
	Exception::Exception(const String& s)
	{
		Exception(s, "", "", 0);
	}
}
#endif
#endif

int main(int argc, char **argv)
{
	OpenViBEVRDemos::COgreVRApplication * app;
	app = new OpenViBEVRDemos::CSpaceInvadersBCI();
	system("pause");
	app->go();
	delete app;

	return 0;
}

/*static void printStart(){
	printf("SPACE INVADERS application started ! \n\n");
	printf("\n");
	printf("                                       \n");
    printf("             ##             ###        \n");
    printf("             ##             ###        \n");
    printf("               ###        ##           \n");
    printf("               ###        ##           \n");
    printf("             ##################        \n");
    printf("             ##################        \n");
    printf("          #####   ########  ######     \n");
    printf("       :::#####:::########::######::   \n");
    printf("       #############################   \n");
    printf("       ###   ##################   ##   \n");
    printf("       ###   ##################   ##   \n");
    printf("       ###   ##             ###   ##   \n");
    printf("       ###   ##             ###   ##   \n");
    printf("               #####°  #####           \n");
    printf("               #####°  #####           \n");
    printf("               »»»»»»  »»»»»           \n");
    printf("               »»»»»¬  »»»»»           \n");
    printf("       ¬¬¬   ¬¬             ¬¬¬   ¬¬   \n");
    printf("       ¬¬¬   ¬¬¬¬¬¬¬¬¬¬¬¬¬¬¬¬¬¬   ¬¬   \n");
    printf("       ¬¬¬¬¬¬¬¬¬¬¬¬¬¬¬¬¬¬¬¬¬¬¬¬¬¬¬¬¬   \n");
    printf("          ¬¬¬¬¬   ¬¬¬¬¬¬¬¬  ¬¬¬¬¬¬     \n");
	printf("                                       \n");
    printf("                                       \n");
    printf("                 »#########            \n");
    printf("                 »#########            \n");
    printf("           #######################     \n");
    printf("         ».#######################»»   \n");
    printf("        ############################   \n");
    printf("        #######     #####    #######   \n");
    printf("        #######     #####    #######   \n");
    printf("        ############################   \n");
    printf("        ############################   \n");
    printf("               #####     ####          \n");
    printf("             ::##:¬¬:::::¬¬##:::       \n");
    printf("             ####.  #####  #####       \n");
    printf("        #####                   ####   \n");
    printf("        #####                   ####   \n");
    printf("        »»»»»                   »»»»   \n");
    printf("        »»»»»                   »»»»   \n");
    printf("             ¬¬¬¬   ¬¬¬¬¬  ¬¬¬¬»       \n");
    printf("               ¬¬¬¬¬     ¬¬¬¬          \n");
    printf("        ¬¬¬¬¬¬¬¬¬¬¬¬¬¬¬¬¬¬¬¬¬¬¬¬¬¬¬¬   \n");
    printf("    ¬¬¬¬¬¬           ¬¬¬¬¬¬¬     ¬¬¬¬  \n");
	printf("                                       \n");
    printf("                                       \n");
	printf("                  ######               \n");
    printf("                  ######               \n");
    printf("               ###########°            \n");
    printf("               ###########°            \n");
    printf("             #################         \n");
    printf("             #################         \n");
    printf("          ######   ######   #####      \n");
    printf("          ######   ######   #####      \n");
    printf("          #######################      \n");
    printf("          #######################      \n");
    printf("                ###      ##°           \n");
    printf("                ###      ##°           \n");
    printf("             ###   ######   ##         \n");
    printf("             ###   ######   ##         \n");
    printf("          ###   ###      ##°  ###      \n");
    printf("          ###   ###      ##°  ###      \n");
    printf("          »»»   »»»      »»»  »»»      \n");
    printf("          »»»   »»»      »»¬  »»»      \n");
    printf("             »¬¬   ¬¬¬¬¬»   ¬¬         \n");
    printf("                ¬¬¬      ¬¬¬           \n");
    printf("                ¬¬¬      ¬¬            \n");
    printf("       ¬¬¬¬¬¬¬         ¬¬¬¬¬¬¬¬¬¬¬¬¬¬¬ \n");
	printf("\n");
}*/
