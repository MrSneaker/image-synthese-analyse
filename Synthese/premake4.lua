dofile "gkit.lua"

 -- description des projets		 
projects = {
	"tp1",
	"tp2",
}

for i, name in ipairs(projects) do
    project(name)
        language "C++"
        kind "ConsoleApp"
        targetdir "bin"
        files ( gkit_files )
        files { "projets/" .. name .. ".cpp" }
end
