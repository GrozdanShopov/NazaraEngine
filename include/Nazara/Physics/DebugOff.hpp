// Copyright (C) 2014 J�r�me Leclercq
// This file is part of the "Nazara Engine - Physics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

// On suppose que Debug.hpp a d�j� �t� inclus, tout comme Config.hpp
#if NAZARA_PHYSICS_MANAGE_MEMORY
	#undef delete
	#undef new
#endif
