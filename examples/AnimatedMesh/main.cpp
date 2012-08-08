#include <Nazara/Core/Clock.hpp>
#include <Nazara/Math/Quaternion.hpp>
#include <Nazara/Math/Vector3.hpp>
#include <Nazara/Renderer/ContextParameters.hpp>
#include <Nazara/Renderer/Renderer.hpp>
#include <Nazara/Renderer/RenderWindow.hpp>
#include <Nazara/Renderer/Shader.hpp>
#include <Nazara/Renderer/Texture.hpp>
#include <Nazara/Utility/Image.hpp>
#include <Nazara/Utility/Mesh.hpp>
#include <Nazara/Utility/StaticMesh.hpp>
#include <iostream>
#include <map>

// Une structure pour contenir nos informations (Cette structure est tr�s simpliste)
struct Model
{
	NzMatrix4f matrix; // Les transformations subies par le mod�le
	NzMesh mesh; // Le mesh
	NzTexture texture; // Sa texture
};

struct AnimatedModel : public Model
{
	// Quelques variables pour l'animation
	const NzSequence* currentSequence = nullptr; // La s�quence en cours
	float interpolation = 0.f; // La valeur de l'interpolation ([0..1], si d�passe 1, on passe � la frame suivante)
	unsigned int currentFrame = 0; // La premi�re frame
	unsigned int nextFrame; // La seconde frame, l'animation est interpoll�e entre ces deux-l�
};

void AnimateModel(AnimatedModel& moedel, float elapsed);
bool CreateCheckerTexture(NzTexture* texture);
bool CreateFloorMesh(NzMesh* mesh);
void DrawModel(const Model& model);
void SetSequence(AnimatedModel& model, const NzString& name);

int main()
{
	// Tout d'abord on affiche les instructions
	std::cout << "Camera controls: ZQSD" << std::endl;
	std::cout << "Dr. Freak controls: Up, down, left and right" << std::endl;
	std::cout << "Escape to quit" << std::endl;

	// Cette ligne active le mode de compatibilit� d'OpenGL lors de l'initialisation de Nazara (N�cessaire pour le shader)
	NzContextParameters::defaultCompatibilityProfile = true;

	// Maintenant nous initialisons le Renderer (Qui initialisera le noyau ainsi que le module utlitaire)
	// Cette �tape est obligatoire pour beaucoup de classes
	NzInitializer<NzRenderer> renderer;
	if (!renderer)
	{
		// �a n'a pas fonctionn�, le pourquoi se trouve dans le fichier NazaraLog.log
		std::cout << "Failed to initialize Nazara, see NazaraLog.log for further informations" << std::endl;
		std::getchar(); // On laise le temps de voir l'erreur
		return EXIT_FAILURE;
	}

	// Maintenant nous pouvons utiliser le moteur comme bon nous semble, tout d'abord nous allons charger les ressources

	// Charger une ressource se fait actuellement manuellement, mais un ResourceManager est � venir
	// Vous initialisez une ressource, et la chargez via sa m�thode LoadFrom[File|Memory|Stream]
	// Note: il est possible de donner des instructions au loader (qui va charger le fichier en ressource) via les ResourceParameters
	NzMeshParams parameters;

	// Le loader doit-il automatiquement charger les animations ?
	// Attention, ce param�tre poss�de une signification diff�rente selon le type d'animation du mesh.
	// -Pour les animations keyframe (image-cl�), c'est la seule et unique fa�on de charger les animations, �tant donn�
	// qu'elles sont fourniees avec le mesh.
	// -Pour les animations squelettiques, le loader ne fera que charger automatiquement l'animation associ�e au mesh s'il le peut
	// Dans les deux cas, les param�tres d'animations (parameters.animation) seront utilis�s
	parameters.loadAnimations = true; // Vaut true par d�faut

	// Pour qu'un mesh puisse �tre rendu, il doit �tre stock� du c�t� de la carte graphique (Hardware), mais il est parfois utile de
	// le stocker c�t� RAM, par exemple pour le moteur physique. En sachant qu'il est facile de changer le stockage d'un buffer.
	parameters.storage = nzBufferStorage_Hardware; // Vaut nzBufferStorage_Hardware par d�faut

	AnimatedModel drfreak;
	if (!drfreak.mesh.LoadFromFile("resources/drfreak.md2", parameters)) // On charge notre bon vieux docteur avec les param�tres de chargement.
	{
		// Le chargement n'a pas fonctionn�, le mod�le est peut-�tre corrompu/non-support�, ou alors n'existe pas.
		std::cout << "Failed to load mesh" << std::endl;
		std::getchar(); // On laise le temps de voir l'erreur
		return EXIT_FAILURE;
	}

	if (!drfreak.mesh.HasAnimation()) // Le mesh poss�de-t-il des animations ?
	{
		// Cette d�mo n'a aucun int�r�t sans animations
		std::cout << "Mesh has no animation" << std::endl;
		std::getchar();
		return EXIT_FAILURE;
	}

	SetSequence(drfreak, "stand");

	// Il est possible que le mesh poss�de un ou plusieurs skin, nous utiliserons cette information pour charger une texture
	if (drfreak.mesh.HasSkin())
	{
		// Contrairement aux autres ressources, la texture n'est pas critique
		if (drfreak.texture.LoadFromFile("resources/" + drfreak.mesh.GetSkin()))
			drfreak.texture.SetFilterMode(nzTextureFilter_Bilinear); // Appliquons-lui un filtrage bilin�aire
		else
			std::cout << "Failed to load texture" << std::endl;
	}

	if (!drfreak.texture.IsValid()) // Les m�thodes Resource::IsValid indiquent si la ressource a �t� correctement cr��e
	{
		std::cout << "Creating checker texture for mesh" << std::endl;

		if (!CreateCheckerTexture(&drfreak.texture))
		{
			std::cout << "Failed to create mesh texture" << std::endl;
			std::getchar();
			return EXIT_FAILURE;
		}
	}

	// Nous cr�ons maintenant notre sol
	Model floor;
	if (!CreateFloorMesh(&floor.mesh))
	{
		std::cout << "Failed to create floor" << std::endl;
		std::getchar();
		return EXIT_FAILURE;
	}

	if (!CreateCheckerTexture(&floor.texture))
	{
		std::cout << "Failed to create floor texture" << std::endl;
		std::getchar();
		return EXIT_FAILURE;
	}

	// Le sol ne subit aucune transformation
	floor.matrix.MakeIdentity();

	// Pour effectuer un rendu, il faut que la carte graphique sache quoi faire.
	// Les shaders sont de petits programmes qui donnent des instructions � la carte graphique lors du pipeline.
	// Ils sont aujourd'hui indispensables pour un rendu 3D, mais sont tr�s utiles pour divers effets !
	// Il existe plusieurs langages de shaders, GLSL pour OpenGL, HLSL pour Direct3D et Cg qui peut �tre utilis� pour les deux.
	// Le Renderer de Nazara utilise OpenGL, par cons�quent nous utiliserons le GLSL
	// La m�thode NzShader::IsLanguageSupported permet de savoir si un langage est support�.
	NzShader shader;
	if (!shader.Create(nzShaderLanguage_GLSL))
	{
		std::cout << "Failed to load shader" << std::endl;
		std::getchar();
		return EXIT_FAILURE;
	}

	// Une fois le shader cr��, nous devons lui sp�cifier les codes sources de nos shaders
	// Pour notre exemple nous prendrons un shader tr�s simple
	// Un shader doit obligatoirement poss�der au moins deux codes, un pour le fragment shader et un pour le vertex shader
	if (!shader.LoadFromFile(nzShaderType_Fragment, "shaders/basic.frag"))
	{
		std::cout << "Failed to load fragment shader from file" << std::endl;
		// � la diff�rence des autres ressources, le shader poss�de un log qui peut indiquer les erreurs en cas d'�chec
		std::cout << "Log: " << shader.GetLog() << std::endl;
		std::getchar();
		return EXIT_FAILURE;
	}

	// Maintenant le vertex shader
	if (!shader.LoadFromFile(nzShaderType_Vertex, "shaders/basic.vert"))
	{
		std::cout << "Failed to load vertex shader from file" << std::endl;
		std::cout << "Log: " << shader.GetLog() << std::endl;
		std::getchar();
		return EXIT_FAILURE;
	}

	// Une fois le code source de nos shaders charg�, nous pouvons le compiler, afin de le rendre utilisable
	if (!shader.Compile())
	{
		std::cout << "Failed to compile shader" << std::endl;
		std::cout << "Log: " << shader.GetLog() << std::endl;
		std::getchar();
		return EXIT_FAILURE;
	}

	// Nos ressources sont charg�es, et c'est bien beau, mais il nous faudrait une fen�tre pour afficher tout �a
	// Window repr�sente une fen�tre singuli�re, pour y effectuer un rendu il nous faut une RenderWindow
	// Tout d'abord, sa taille, disons celle du bureau divis� par deux
	NzVideoMode mode = NzVideoMode::GetDesktopMode();
	mode.width /= 2;
	mode.height /= 2;

	NzString title = "Nazara Demo - AnimatedMesh";

	NzRenderWindow window;
	// Le premier argument d�finit la taille de rendu de la fen�tre (Si elle poss�de des bordures elle sera l�g�rement plus grande)
	// Le deuxi�me argument est le titre de la fen�tre lors de sa cr�ation, vous pouvez le modifier � tout moment via window.SetTitle
	// Le troisi�me argument repr�sente la d�coration de la fen�tre, sa bordure, ses boutons.
	// Attention que cela permet � la fen�tre d'envoyer des �v�nements, et de changer sa taille
	// Par d�faut le troisi�me argument vaut nzWindowStyle_Default (Bordure + Bouton de fermeture + Redimensionnement)
	if (!window.Create(mode, title, nzWindowStyle_Default))
	{
		std::cout << "Failed to create window" << std::endl;
		std::getchar();
		return EXIT_FAILURE;
	}

	// On cache le curseur
	window.SetCursor(nzWindowCursor_None);

	// Nous limitons les FPS � 100
	window.SetFramerateLimit(100);

	// La matrice de projection d�finit la transformation du vertice 3D � un point 2D
	NzRenderer::SetMatrix(nzMatrixType_Projection, NzMatrix4f::Perspective(NzDegrees(70.f), static_cast<float>(window.GetWidth())/window.GetHeight(), 1.f, 1000.f));

	// Notre fen�tre est cr��e, cependant il faut s'occuper d'elle, pour le rendu et les �v�nements
	NzClock secondClock, updateClock; // Des horloges pour g�rer le temps
	unsigned int fps = 0; // Compteur de FPS

	// Quelques variables pour notre improvisation de physique
	float groundPos = drfreak.mesh.GetAABB().GetMinimum().y; // Pas tr�s exact
	NzVector3f modelPos(0.f, groundPos, -50.f);
	NzVector3f modelVel(0.f, 0.f, 0.f);
	NzQuaternionf modelRot(NzEulerAnglesf(0.f, 0.f, 0.f)); // Les angles d'eulers sont bien plus facile � se repr�senter
	float speed = 60.f;

	// Nous initialisons la matrice
	drfreak.matrix = NzMatrix4f::Rotate(modelRot) * NzMatrix4f::Translate(modelPos);

	// Notre cam�ra
	NzVector3f camPos(0.f, 25.f, -20.f);
	NzEulerAnglesf camRot(0.f, 0.f, 0.f);
	NzMatrix4f camMatrix = NzMatrix4f::Translate(camPos);
	float camSpeed = 2.f;
	float sensitivity = 0.5;

	// Derni�re �tape, nos touches

	// Chaque touche fera bouger
	struct Movement
	{
		NzVector3f direction; // La direction
		NzQuaternionf rotation; // La rotation du mod�le
	};

	std::map<NzKeyboard::Key, Movement> movements;
	movements[NzKeyboard::Up] = Movement{NzVector3f(0.f, 0.f, -1.f), NzQuaternionf(NzEulerAnglesf(0.f, 180.f, 0.f))};
	movements[NzKeyboard::Down] = Movement{NzVector3f(0.f, 0.f, 1.f), NzQuaternionf(NzEulerAnglesf(0.f, 0.f, 0.f))};
	movements[NzKeyboard::Left] = Movement{NzVector3f(-1.f, 0.f, 0.f), NzQuaternionf(NzEulerAnglesf(0.f, 90.f, 0.f))};
	movements[NzKeyboard::Right] = Movement{NzVector3f(1.f, 0.f, 0.f), NzQuaternionf(NzEulerAnglesf(0.f, -90.f, 0.f))};
	NzKeyboard::Key currentKey = NzKeyboard::Undefined;

	// Quelques bool�ens
	bool camMode = true;
	bool windowOpen = true;
	while (windowOpen)
	{
		// Ici nous g�rons les �v�nements
		NzEvent event;
		while (window.PollEvent(&event)) // Avons-nous un �v�nement dans la file ?
		{
			// Nous avons un �v�nement
			switch (event.type) // De quel type est cet �v�nement ?
			{
				case nzEventType_Quit: // L'utilisateur/L'OS nous a demand� de terminer notre ex�cution
					windowOpen = false; // Nous fermons alors la boucle
					break;

				case nzEventType_MouseMoved:
				{
					// Si nous ne sommes pas en mode cam�ra, on ne traite pas l'�v�nement
					if (!camMode)
						break;

					// On modifie l'angle de la cam�ra gr�ce au d�placement relatif de la souris
					camRot.yaw = NzNormalizeAngle(camRot.yaw - event.mouseMove.deltaX*sensitivity);

					// Pour �viter les loopings, on restreint les angles
					camRot.pitch = NzClamp(camRot.pitch + event.mouseMove.deltaY*sensitivity, -90.f, 90.f);

					// La matrice vue repr�sente les transformations effectu�es par la cam�ra
					// On recalcule la matrice de la cam�ra et on l'envoie au renderer
					NzRenderer::SetMatrix(nzMatrixType_View, NzMatrix4f::Translate(camPos) * NzMatrix4f::Rotate(camRot));

					// Pour �viter que le curseur ne sorte de l'�cran, nous le renvoyons au centre de la fen�tre
					NzMouse::SetPosition(window.GetWidth()/2, window.GetHeight()/2, window);
					break;
				}

				case nzEventType_MouseButtonPressed:
					if (event.mouseButton.button == NzMouse::Left)
					{
						// L'utilisateur vient d'appuyer sur le bouton left de la souris
						// Nous allons inverser le mode cam�ra et montrer/cacher le curseur en cons�quence
						if (camMode)
						{
							camMode = false;
							window.SetCursor(nzWindowCursor_Default);
						}
						else
						{
							camMode = true;
							window.SetCursor(nzWindowCursor_None);
						}

					}
					break;

				case nzEventType_Resized: // L'utilisateur a chang� notre taille, le coquin !
					NzRenderer::SetViewport(NzRectui(0, 0, event.size.width, event.size.height)); // Adaptons l'affichage

					// Il nous faut aussi mettre � jour notre matrice de projection
					NzRenderer::SetMatrix(nzMatrixType_Projection, NzMatrix4f::Perspective(NzDegrees(70.f), static_cast<float>(event.size.width)/event.size.height, 1.f, 1000.f));
					break;

				case nzEventType_KeyPressed:
					if (!event.key.repeated) // Si la touche n'est pas r�p�t�e
					{
						auto it = movements.find(event.key.code);
						if (it != movements.end())
						{
							// Si la touche est une touche de mouvement
							SetSequence(drfreak, "run"); // On anime le personnage pour qu'il ait une animation de d�placement

							modelRot = it->second.rotation; // On change la rotation du mod�le
							drfreak.matrix = NzMatrix4f::Rotate(modelRot) * NzMatrix4f::Translate(modelPos); // On recalcule sa matrice
							modelVel = it->second.direction * speed; // On change la vitesse de d�placement
							currentKey = event.key.code;
						}
					}

					if (event.key.code == NzKeyboard::Escape)
						windowOpen = false;

					break;

				case nzEventType_KeyReleased:
					if (event.key.code == currentKey)
					{
						SetSequence(drfreak, "stand");
						modelVel = NzVector3f(0.f); // On arr�te le d�placement
						break;
					}

					break;

				default: // Les autres �v�nements, on s'en fiche
					break;
			}
		}

		// On active le shader et param�trons le rendu
		NzRenderer::SetShader(&shader);
		NzRenderer::Enable(nzRendererParameter_DepthTest, true);

		NzRenderer::SetClearColor(128, 128, 128);
		NzRenderer::Clear(nzRendererClear_Color | nzRendererClear_Depth);

		if (updateClock.GetMilliseconds() >= 1000/60) // 60 fois par seconde
		{
			float elapsedTime = updateClock.GetSeconds(); // Le temps depuis la derni�re mise � jour

			// D�placement de la cam�ra
			static const NzVector3f forward(NzVector3f::UnitZ());
			static const NzVector3f left(NzVector3f::UnitX());
			static const NzVector3f up(NzVector3f::UnitY());

			// Notre rotation sous forme de quaternion nous permet de tourner un vecteur
			NzQuaternionf quaternion(camRot);

			// Par exemple ici, quaternion * forward nous permet de r�cup�rer le vecteur de la direction "avant"
			if (NzKeyboard::IsKeyPressed(NzKeyboard::Z))
				camPos += quaternion * forward * camSpeed;

			if (NzKeyboard::IsKeyPressed(NzKeyboard::S))
				camPos -= quaternion * forward * camSpeed;

			if (NzKeyboard::IsKeyPressed(NzKeyboard::Q))
				camPos += quaternion * left * camSpeed;

			if (NzKeyboard::IsKeyPressed(NzKeyboard::D))
				camPos -= quaternion * left * camSpeed;

			// En revanche, ici la hauteur est toujours la m�me, peu importe notre orientation
			if (NzKeyboard::IsKeyPressed(NzKeyboard::Space))
				camPos += up * camSpeed;

			if (NzKeyboard::IsKeyPressed(NzKeyboard::LControl))
				camPos -= up * camSpeed;

			// Oui le quaternion et la matrice sont calcul�s m�me si la cam�ra ne bouge pas
			// C'est une limitation de mon impl�mentation, qui ne sera pas pr�sente une fois les NzSceneNode int�gr�s
			NzRenderer::SetMatrix(nzMatrixType_View, NzMatrix4f::Translate(camPos) * NzMatrix4f::Rotate(camRot));

			// Animation
			AnimateModel(drfreak, elapsedTime);
			updateClock.Restart();

			// "Physique"
			if (modelVel != NzVector3f::Zero())
			{
				modelPos += modelVel * elapsedTime;
				/*if (jumping)
				{
					velocity.y -= 500.f * elapsedTime; // Un simulacre de gravit�
					if (modelPos.y <= groundPos)
					{
						// On stoppe net
						modelPos.y = groundPos;
						velocity.y = 0.f;
						jumping = false;
						SetSequence(drfreak, "stand");
					}
				}*/

				// Mise � jour de la matrice
				drfreak.matrix = NzMatrix4f::Rotate(modelRot) * NzMatrix4f::Translate(modelPos);
			}
		}

		// Affichage des meshs
		DrawModel(floor);

		// Notre Dr. Freak poss�de des normales, nous pouvons alors culler les faces qu'on ne voit pas
		NzRenderer::Enable(nzRendererParameter_FaceCulling, true);

		DrawModel(drfreak);

		NzRenderer::Enable(nzRendererParameter_FaceCulling, false);

		window.Display(); // Nous mettons � jour l'�cran

		fps++;

		// Toutes les secondes
		if (secondClock.GetMilliseconds() >= 1000)
		{
			window.SetTitle(title + " (FPS: " + NzString::Number(fps) + ')');
			fps = 0;
			secondClock.Restart();
		}
	}

    return EXIT_SUCCESS;
}

void AnimateModel(AnimatedModel& model, float elapsed)
{
	model.interpolation += model.currentSequence->framePerSecond * elapsed;
	while (model.interpolation > 1.f)
	{
		model.interpolation -= 1.f;

		model.currentFrame = model.nextFrame;
		if (++model.nextFrame > model.currentSequence->lastFrame)
			model.nextFrame = model.currentSequence->firstFrame;
	}

	model.mesh.Animate(model.currentFrame, model.nextFrame, (NzKeyboard::IsKeyPressed(NzKeyboard::A)) ? 0.f : model.interpolation);
}

bool CreateCheckerTexture(NzTexture* texture)
{
	NzImage image;
	// Nous cr��ons une image 2D, au format RGBA8 de dimensions 128*128 (8 blocs de 16 pixels de c�t�)
	if (!image.Create(nzImageType_2D, nzPixelFormat_RGBA8, 8*16, 8*16))
	{
		// Ne devrait pas arriver (La cr�ation d'une image ne peut �chouer que si l'un des argument est incorrect)
		std::cout << "Failed to create image, this means a bug has been found in Nazara" << std::endl;
		return false;
	}

	// Pour modifier les pixels, nous pouvons acc�der directement � ces derniers avec GetPixels(), ou bien � un pixel
	// via [Get|Set]PixelColor, mais pour cette occasion nous utiliserons une m�thode bien pratique, Fill.
	unsigned int blockCountX = image.GetWidth()/16;
	unsigned int blockCountY = image.GetHeight()/16;
	for (unsigned int x = 0; x < blockCountX; ++x)
	{
		for (unsigned int y = 0; y < blockCountY; ++y)
		{
			// Une belle texture de damier
			NzColor color = (x%2 == y%2) ? NzColor::White : NzColor::Black;
			// Fill remplit une zone de la texture avec une couleur
			image.Fill(color, NzRectui(x*16, y*16, 16, 16));
		}
	}

	if (!texture->LoadFromImage(image)) // Nous cr�ons notre texture depuis notre image
	{
		// Nous n'avons vraiment pas beaucoup de chance..
		std::cout << "Failed to load image" << std::endl;
		return false;
	}

	texture->SetAnisotropyLevel(NzRenderer::GetMaxAnisotropyLevel()); // Un filtrage anisotropique pour la texture
	texture->SetWrapMode(nzTextureWrap_Repeat); // Si les coordonn�es de texture d�passent 1.f, la texture sera r�p�t�e

	return true;
}

bool CreateFloorMesh(NzMesh* mesh)
{
	// Cette fonction cr�� un mesh statique simpliste pour servir de sol

	// Nous cr�ons un mesh statique
	if (!mesh->Create(nzAnimationType_Static))
	{
		// L'�chec est techniquement impossible mais le moteur �tant en constante �volution ...
		std::cout << "Failed to create mesh" << std::endl;
		return false;
	}

	// Les vertex declaration ont pour seul but de d�crire l'agencement d'un vertex buffer
	// Elles sont compos�es de VertexElement, chacun d�crivant un �l�ment du buffer
	NzVertexDeclaration* declaration = new NzVertexDeclaration;

	// Il y a cinq param�tres diff�rents (stream, usage, type, offset, usageIndex)
	// -Stream: � quoi serviront les donn�es ? � d�finir des sommets (nzElementStream_VertexData) ou � l'instancing (nzElementStream_InstancedData)
	// -Usage: Comment cette donn�e doit-elle �tre envoy�e au shader
	// -Type: Comment sont stock�es ces donn�es ? (Un triplet de float ? Deux double ? ..)
	// -Offset: La position de la donn�e dans le buffer (les donn�es sont entrelac�es)
	// -UsageIndex: Pour les coordonn�es de texture, d�finit l'indice de texture utilis�.
	NzVertexElement elements[2];
	elements[0].usage = nzElementUsage_Position; // Notre premier �l�ment sera la position des vertices
	elements[0].offset = 0; // Celles-ci sont plac�es au d�but
	elements[0].type = nzElementType_Float3; // Sont compos�es de trois flottants

	elements[1].usage = nzElementUsage_TexCoord;
	elements[1].offset = 3*sizeof(float);
	elements[1].type = nzElementType_Float2;

	if (!declaration->Create(elements, 2))
	{
		// Nos �l�ments sont invalides !
		std::cout << "Failed to create vertex declaration" << std::endl;
		return false;
	}

	// Nous cr�ons ensuite un buffer de 4 vertices (le second argument pr�cise l'espace pris par chaque vertex), le stockage
	// Et nous indiquons que nous n'y toucherons plus
	NzVertexBuffer* buffer = new NzVertexBuffer(4, declaration->GetStride(nzElementStream_VertexData), nzBufferStorage_Hardware, nzBufferUsage_Static);

	// Doit respecter la declaration
	float vertices[] =
	{
		// Vertex 1
		-1000.f, 0.f, -1000.f, // Position
		0.f, 0.f,          // UV

		// Vertex 2
		-1000.f, 0.f, 1000.f, // Position
		0.f, 10.f,          // UV

		// Vertex 3
		1000.f, 0.f, -1000.f, // Position
		10.f, 0.f,          // UV

		// Vertex 4
		1000.f, 0.f, 1000.f, // Position
		10.f, 10.f           // UV
	};

	// Afin de modifier un buffer, il nous faut soit le verrouiller (acc�s bas-niveau),  soit le remplir (acc�s de plus haut niveau)
	if (!buffer->Fill(vertices, 0, 4)) // Nous remplissons � partir de l'index 0, et nous envoyons 4 vertices
	{
		std::cout << "Failed to fill buffer" << std::endl;
		return false;
	}

	NzStaticMesh* subMesh = new NzStaticMesh(mesh);
	if (!subMesh->Create(declaration, buffer))
	{
		std::cout << "Failed to create subMesh" << std::endl;
		return false;
	}

	subMesh->SetPrimitiveType(nzPrimitiveType_TriangleStrip);

	// On ajoute le submesh au mesh
	mesh->AddSubMesh(subMesh);

	// Nos ressources sont notifi�es utilis�es par le mesh et le submesh, nous pouvons les rendre �ph�m�res.
	// Les ressources seront donc automatiquement lib�r�es lorsque plus aucune classe n'en aura besoin
	buffer->SetPersistent(false);
	declaration->SetPersistent(false);
	subMesh->SetPersistent(false); // Pour le submesh, c'est d�j� � false � la base

	return true;
}

void DrawModel(const Model& model)
{
	// La matrice world est celle qui repr�sente les transformations du mod�le
	NzRenderer::SetMatrix(nzMatrixType_World, model.matrix);

	NzShader* shader = NzRenderer::GetShader();// On r�cup�re le shader du rendu
	shader->SendTexture(shader->GetUniformLocation("texture"), &model.texture);

	// Un mesh est divis� en plusieurs submeshes
	unsigned int subMeshCount = model.mesh.GetSubMeshCount();
	for (unsigned int i = 0; i < subMeshCount; ++i)
	{
		// On r�cup�re le submesh
		const NzSubMesh* subMesh = model.mesh.GetSubMesh(i);

		// On param�tre le Renderer avec ses donn�es
		NzRenderer::SetIndexBuffer(subMesh->GetIndexBuffer());
		NzRenderer::SetVertexBuffer(subMesh->GetVertexBuffer());
		NzRenderer::SetVertexDeclaration(subMesh->GetVertexDeclaration());

		// On fait le rendu
		NzRenderer::DrawPrimitives(subMesh->GetPrimitiveType(), 0, subMesh->GetVertexCount());
	}
}

void SetSequence(AnimatedModel& model, const NzString& sequenceName)
{
	// On r�cup�re l'animation du mesh
	const NzAnimation* animation = model.mesh.GetAnimation();

	// Nous nous basons sur l'assertion que la s�quence existe (Chose que nous pouvons tester avec HasSequence())
	model.currentSequence = animation->GetSequence(sequenceName);

	// Pour avoir une interpolation entre la s�quence pr�c�dente et celle-ci, nous n'affectons que nextFrame
	model.nextFrame = model.currentSequence->firstFrame;
}
