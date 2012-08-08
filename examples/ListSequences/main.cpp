#include <Nazara/Utility/Mesh.hpp>
#include <Nazara/Utility/Utility.hpp>
#include <iostream>

int main()
{
	// Pour charger des ressources, il est imp�ratif d'initialiser le module utilitaire
	NzInitializer<NzUtility> utility;
	if (!utility)
	{
		// �a n'a pas fonctionn�, le pourquoi se trouve dans le fichier NazaraLog.log
		std::cout << "Failed to initialize Nazara, see NazaraLog.log for further informations" << std::endl;
		std::getchar(); // On laise le temps de voir l'erreur
		return EXIT_FAILURE;
	}

	// Le Renderer n'�tant pas charg�, nous devons indiquer que nous d�sirons un stockage software de notre mesh
	NzMeshParams parameters;
	parameters.storage = nzBufferStorage_Software;

	NzMesh drfreak;
	if (!drfreak.LoadFromFile("resources/drfreak.md2", parameters))
	{
		std::cout << "Failed to load Dr. Freak's mesh" << std::endl;
		std::getchar();
		return EXIT_FAILURE;
	}

	if (drfreak.HasAnimation()) // Si le mesh poss�de des animations
	{
		std::cout << "Mesh has animation" << std::endl;

		// Un objet NzAnimation repr�sente un ensemble d'informations sur des animations
		const NzAnimation* animation = drfreak.GetAnimation();

		// Une s�quence d�finit une "action", chaque s�quence poss�de un nom, une frame de d�part, une d'arriv�e ainsi qu'un framerate
		unsigned int sequenceCount = animation->GetSequenceCount();
		std::cout << sequenceCount << " sequences:" << std::endl;
		for (unsigned int i = 0; i < sequenceCount; ++i)
			std::cout << "-" << (i+1) << ": " << animation->GetSequence(i)->name << std::endl;
	}
	else
		std::cout << "Mesh has no animation" << std::endl;

	std::getchar(); // Une attente pour avoir le temps de lire :-)

	// Le module utilitaire et le mesh sont d�charg�s automatiquement
	return EXIT_SUCCESS;
}
