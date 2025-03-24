
class HeaderFile
    pro init(@filename)
        @level_offsets = []
        @end_of_data = nil
        f = File.new(@filename)
        @size = f.size()
        data = File.read()
        index = Index(data)
        @compression_mark = index.readUINT16LE()
        for i in 0..99 do
            n = index.readUINT32LE()
            if n == 0xFFFFFFFF then
                @end_of_data = index as int
                break
            end
            @level_offsets << n
        loop
    end
    
    pro info()
        print("------------------------------------")
        print("[Header File Information]")
        print("------------------------------------")
        print("File name = ${@filename}")
        print("File size = ${@size}")
        print("Compression mark : " + hex(@compression_mark) + " (dec = ${@compression_mark}")
        print("Number of levels : ${@level_offsets.size()}")
        print("Number of bytes read = ${@self.end_of_data} / ${@size}")
        print("List of level offsets = ")
        for i, v in level_offsets do
            print("    ", i, hex(v), string.format("%05d", v))
        loop
        print("------------------------------------")
    end
end
