function encodeShaders()
	local targets = os.matchdirs("../src/Nazara/Renderer/Shaders/*")
	for k, targetPath in pairs(targets) do
		local shaders = os.matchfiles(targetPath .. "/*")
		for k, filePath in pairs(shaders) do
			local ext = filePath:sub(-5)
			if (ext == ".frag" or ext == ".geom" or ext == ".vert") then
				local shader, err = io.open(filePath, "r")
				if (not shader) then
					error("Failed to read shader file: " .. err)
				end

				local header, err = io.open(filePath .. ".h", "w+")
				if (not header) then
					error("Failed to create header file: " .. err)
				end

				for line in shader:lines() do
					line = line .. "\n"
					local length = line:len()
					for i = 1, length do
						local byte = line:sub(i, i):byte()
						if (byte >= 128) then
							byte = byte - 256
						end
						header:write(string.format("%d,", byte))
					end
				end

				header:close()
				shader:close()
			end
		end
	end
end

newaction
{
	trigger     = "encodeshaders",
	description = "Generate a includable header version of shaders",
	execute     = encodeShaders
}
