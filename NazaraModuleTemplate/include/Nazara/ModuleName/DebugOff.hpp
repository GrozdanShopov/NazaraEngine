// Copyright (C) 2013 AUTHORS
// This file is part of the "Nazara Engine - Module name"
// For conditions of distribution and use, see copyright notice in Config.hpp

#if NAZARA_MODULENAME_MEMORYLEAKTRACKER || defined(NAZARA_DEBUG)
	#undef delete
	#undef new
#endif
