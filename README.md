# editjson
```py3

# editjson
# 2022-05-31
# check key no ends %$~ in json data.

# schema
schema = {
    "name%$name" 
    "power" {
        "army%$" 
    }
} 

# script
script = {
    $name == "AAA" # all comparison is cond.
    $ = $ * 2      # to update...
} 

# input
{
    "json_data" : { "name" : "AAA",  "power" : { "army" : 20 } }  
}
# result
{
    "json_data" : { "name" : "AAA",  "power" : { "army" : 40 } }  
}

# 2022-05-27

# goal : To edit json-like? data fastly and simplely, using thread.

# here... json-like... exist 1)object, exist 2)array, exist 3)nested structure!

# for Iterate? Job.

# scenario. for game data.
# countries <- has many provinces.

## simple? coding, and modding.?

# json example. cf) # ?
[ 
    {
        "countries" : [
            {
                "name" : "AAA",
                "power" : {
                    "army" : 100,
                    "air" : 100,
                    "navy" : 100
                }
            },
            {
                "name" : "BBB",
                "power" : {
                    "army" : 50,
                    "air" : 50,
                    "navy" : 50
                }
            },
            ...
        ]
        "provinces" : {
            "-1" : { # chk get "-1" ? cf) ~ : [ { "id" = "-1",  "owner" : "AAA", ...
                "owner" : "AAA",
                "trade_power"  : 10
            },
            "-2" : {
                "owner" : "BBB",
                "trade_power"  : 10
            },
            "-3" : {
                "owner" : "AAA",
                "trade_power"  : 50
            },
            ...
        }
    }
]

# 0. json-path
$root
$root.[].{}."countries"

# 1. chk my? json-schema?
schema = {
    "name" : "%$name", # condition.
    "power" : {
        "army" : "%$" 
    }
}

# 2. script.
script = {
    $name == "AAA" # all comparison is cond.
    let x = $ * 2
    $ = x + 1      # to update...
}   

# 3. work.
work = {
    name = test
    
    where = $root.[].{}."countries".$all
    
    schema = {
        "name" : "%$name", 
        "power" : {
            "army" : "%$" # case no need air, no need navy.
        }
    }
    
    script = { # per line...?
        # $~ <- schema,
        # else <- param, local. no dup?
        $name == "AAA" # all comparison is cond.
        $ = $ * 2      # to update...
    }   
}

# 3. work, and sub_work??
[
    {
        "aaa" : {
            {
                "name" : "test",
                             
                "int_values" : [ 1, 2, 3, 4, 5 ]
            },
            {
                "name" : "test",
                             
                "int_values" : [ 2, 4, 6, 8, 10 ]
            },
            ...
        }
    }
]

work = {
    name = test
    
    # option = { } "flag" : "true or false?"
    
    # param = [ ]
    
    sub_work = { # chk need more thinking...
        name = sub_test
        where = ~~
        #param = [  ] # parameter.
        schema = {
           ~~
        }
        script = {
            return x = true #param = [ x : null ] x = true # "x" ? x ?
        }
    }
    
    where = $root.[].{}."aaa".$all
    
    schema = { # "name%$var_name"
        "name" : "%$name", # key : value, value, <- value에 $id_name?
        "int_values%$a" : [ # key : { } ,key : [ ] , "%$id_name" : { } , "%$id_name" : [ ]
            # 1개이상?
        ]
    }
    
    script = { # per line...?
        $name == "test" # all comparison is cond.
        sum($a) > 20   # cf) c++ grammar?,  def func  <- put in?
        sub_work().x == true
        let static chk = sub_work2(param1, param2) # static call once, no static call every time.
        # if, else, while, static.
    }   
}

event = {
    id = main
    
    let x = [1 2 3] # pass by ref?
    let y = { "abc" : true }
    
    run = # def run ~~ ?
    [ # sequential
        { # parallel.?
            test(x)
            test2(y)
        }
        test3(x) # wait...
    ]
}

# chk..


# 1. chk my? json-schema?
schema = {
    "name" : "%$name", # condition.
    "power" : {
        "army" : "%$" 
    }
}

# 2. script.
script = {
    $name == "AAA" # all comparison is cond.
    $ = $ * 2      # to update...
    delete($) # remove
    add(this , { "chk" : true }) # push_back.
}   
```
