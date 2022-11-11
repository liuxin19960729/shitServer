print("run lua init.lua")


function OnInit(id)
    print("[lua] main OnInit")
    local ping1 = ShitNet.NewService("ping");
    print("lua] main ping1:" .. ping1)
    local ping2 = ShitNet.NewService("ping");
    print("lua] main ping2:" .. ping2)
    local pong = ShitNet.NewService("ping");
    print("lua] main pong::" .. pong)
    ShitNet.Send(ping1, pong, "p1->pong");
    ShitNet.Send(ping2, pong, "p2->pong");
end

function OnExit()
    print("[lua] main OnExit")
end
