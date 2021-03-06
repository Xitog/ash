local ash
local win = ide.osname == "Windows"

return {
  name = "Ash",
  description = "Ash, a simple language",
  api = {"baselib"},
  frun = function(self,wfilename,rundebug)
    local file = wfilename:GetFullPath()
    local cmd = ('python D:\\Users\\gouteud\\Downloads\\ash.py "%s"'):format(file)
    return CommandLineRun(cmd,self:fworkdir(wfilename),true,false,nil,nil,nil)
  end,
  hasdebugger = false,
}