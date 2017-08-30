ACTION.Name = "EncodeResources"
ACTION.Description = "Generate a includable header version of resources"

ACTION.Function = function ()
	print("Encoding resources ...")
	local startClock = os.clock()
	local modules = os.matchdirs("../src/Nazara/*")
	table.insert(modules, "../SDK/src/NDK")
	for k, modulePath in pairs(modules) do
		local moduleName
		if (modulePath:sub(4, 6) == "src") then
			moduleName = modulePath:sub(15, -1)
		else
			moduleName = "SDK"
		end
		local files = os.matchfiles(modulePath .. "/Resources/**")
		for k, filePath in pairs(files) do
			if (filePath:sub(-2) ~= ".h") then
				local file = filePath:sub(modulePath:len() + 12, -1)
				local resource, err = io.open(filePath, "rb")
				if (not resource) then
					error("Failed to read resource file " .. file .. ": " .. err)
				end

				local resourceContent = resource:read("*a")
				resource:close()

				local contentLength = resourceContent:len()

				local headerContentTable = {}
				for i = 1, contentLength do
					table.insert(headerContentTable, string.format("%d,", resourceContent:byte(i)))
				end
				local headerContent = table.concat(headerContentTable)

				local header, err = io.open(filePath .. ".h", "w+")
				if (not header) then
					error("Failed to create header file for " .. file .. ": " .. err)
				end

				header:write(headerContent)
				header:close()

				print(string.format("%s: %s (raw: %.3g kB, header: %.3g kB)", moduleName, file, contentLength/1024, string.format("%.3g", headerContent:len()/1024)))
			end
		end
	end
	print("Finished (took " .. os.clock() - startClock .. "s)")
end
