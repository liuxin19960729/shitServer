local serviceid;
function OnInit(id)
    print("[lua] ping OnInit id: " .. id);
    serviceid = id;
end

function OnServiceMsg(source, buff)
    print("[lua] ping OnServiceMsg id: " .. serviceid .. " msg: " .. buff);
    if string.len(buff) > 9 then
        ShitNet.Killservice(serviceid)
        return
    end
    ShitNet.Send(serviceid, source, buff .. "lx");
end

function OnExit()
    print("[lua] ping OnInit id: " .. serviceid);
end
