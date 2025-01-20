# editjson
```py3

# editjson

# json example.
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
        ],
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

# 2025-01-20
.pattern {
	"name" : %string
	"power" : {
		"army" : %int
	}	
} Pat 

.func {
	Pat p1 = { 
		"name" : "AAA"
		"power" : { 
			"army" : 10
			"air" : 20 
			"navy" : 20 
		}
	}
	Pat p2 = {
		"name" : "BBB"
		"power" : {
			"army" : 20
		}
	}
	.println(p1)
	.println(p2)

	.goto(%root.0."countries")
    
    #.iterate(Pat, f) # iterate with action?
	while NOT .IsEndOfContainer(%now) {
		.enter()
        
		Pat ref x = %now # Pat& x = %now

        # f(x)
        {
            if x.name == "AAA" {
                x.army = 100
            }
        }
	
        .quit()
		.next()
	}
} main

.action {
	if $name == "AAA" {
		$army = 100
	}
} f

# copy input file into output.json
# clau.exe test.clau input.json
.func {
	.println("#hello clau#")
	.write("output.json") 
} main

# input : json?, output : json?
.func {
    return $ * 2 + 1
}

# 2025-01-15
.pattern {
	"name" : %string
	"power" : {
		"army" : %int
	}	
} Pat 

.action {
	Pat p1 = { 
		"name" : "AAA"
		"power" : { 
			"army" : 10
			"air" : 20 
			"navy" : 20 
		}
	}
	Pat p2 = {
		"name" : "BBB"
		"power" : {
			"army" : 20
		}
	}
	.println(p1)
	.println(p2)

	.goto(%root.0."countries")
	while NOT %now.IsEndOfContainer() {
		.enter()
		Pat ref x = %now # Pat& x = %now
		if x.name == "AAA" {
			x.army = 100
		}
		.quit()
		.next()
	}
	
} main

.action {
	if $name == "AAA" {
		$army = 100
	}
} f

#.goto .iterate <- 내장함수?
.action {
	.iterate(pat, f)
	.goto("countries")
	.goto(%back) # cd .. ?
	#.goto(%root.0."countries"})
} main 

# pattern action matching?

# test.clau
.action {
	.print("hello clau")
	.write("output.json") # copy input file. no change
} main # std::unordered_map<std::string, Action*> action_map;
# clau test.clau "input.json"
# 1. parse json file
# 2. run aciton those name is main

# 2025-01-10
#scj3 - x64 only.
# todo - 주석처리! 
# clau_edit? program to edit json.
# .pattern, .action, .where, .global
# strict ? 

.pattern {
    "name" : %string
    "power" : {
        "army" : %$%int
        "nuke" : %remove_target
    }
} pattern1

.action {
    if $name == "AAA" {
        $ = $ * 2 + 1
    }
} action1

.where {
    %root.[].{"countries"}.[%all] # check...
	%global.path 
} where1

# can access from .action?
.global {
	Int i
	UInt u
	Float f
	String s
	Bool b
	Array array  # ? add(array, $);
	Object object # ? add(object, "key", value);
	Path path
}

.run {
    pattern1 { action1 } in where1
} 

.pattern {
	"x" : %int
	"y" : %int
	"z" : %change_target # insert_target
} f_patt
.action {
	$z = $x + $y 
} f

.action {
	let obj = { "x" : 5 "y" : 6 "z" : 0 }
	f(f_patt, obj)
	print(obj.{"z"})
}


# 2024-12-14

# clau_edit? program to edit json.
# .pattern, .action, .where, .global

.pattern {
    "name" : %string,
    "power" : {
        "army" : %$%int,
        "nuke" : %CAN_BE_REMOVED
    }
} pattern1;

.action {
    if ( $name == "AAA" ) {
        $ = $ * 2 + 1 ;
    }
} action1;

.where {
    %root.[].{}.["countries"].{%all} # check...
} where1;

# can access from .action?
.global {
	[ ] array;  # ? add(array, $);
	{ } object; # ? add(object, "key", value);
	{ } object2; # temp?
}

.run {
    pattern1 { action1 } in where1;
} 

# if object is multi-map? or claujson::Object?
# any, not, all, at_least, at_most, exactly?
.action {
	if any( .object["name"] > 50 ) {
		#
	}
	if all( .object["name"] > 20 ) {
		#
	}
}

## in .where?
.pattern {
	"name" : %$name
} p;
.where {
	.pattern {
		"owner" : %$owner
	} _;
	.action {
		return $owner[] += $owner;
	} __;
	.where {
		%root.[].{}.{"provinces"}.{$all}
	} ___;
	.run {
		_ { __ } in ___; 
	}
	%root.[].{}.["countries"].{ $name in $owner }
} w;
.action {
	print($name, "\n");
} a;
.run {
	p { a } in w;
}

## in .pattern
.pattern {
	"owner" : $owner
} pat1;
.action {
	object[$owner] += %NOW_KEY ;
} act1;
.where {
	%root.[].{}.{"provinces"}.{$all}
} whe1;
.pattern {
	"name" : %$name
} pat2;
.action {
	print($name, object[$name], "\n");
} act2;
.where {
	%root.[].{}.["countries"].{$all}
} whe2;
.run {
	pat1 { act1 } in whe1;
	pat2 { act2 } in whe2;
}

## in .action ##
# %NOW_IDX # NOW <- from .where?
# %NOW_KEY
# %ROUTE # chk.. %all 


# 2024-12-13

# clau_edit? program to edit json.
# .pattern, .action, .where, .global

.pattern {
    "name" : %string,
    "power" : {
        "army" : %$%int,
        "nuke" : %CAN_BE_REMOVED
    }
} pattern1

.action {
    if $name == "AAA" {
        $ = $ * 2 + 1 ;
    }
} action1

.where {
    %root.[].{}.["countries"].{%all} # check...
} where1

# can access from .action?
.global {
	[ ] array  # ? array += $;
	{ } object # ? object += { "key" : value };
}

.run {
    pattern1 { action1 } in where1
} 

# if object is multi-map?
# any, not, all, at_least, at_most, exactly?
.action {
	if any( .object["name"] > 50 ) {
		#
	}
	if all( .object["name"] > 20 ) {
		#
	}
}

## in .action ##
# %NOW_IDX # NOW <- from .where?
# %NOW_KEY
# %ROUTE # chk.. %all 
#


# 2024-12-10
./clau_edit ex1.json 0
# loaded file : "ex1.json"
# ls?, cd?, help?, with utf-8
#----------------------------------
cd []
cd {}
cd "countries"
# 
schema 
{
    "name" : %string,
    # cf) "power" : { "army" : %$%int, "nuke" : %CAN_BE_REMOVED } 
    "power"."army" : %$%int,
    "power"."nuke" : %CAN_BE_REMOVED
    #, "economy" : %NONE%int # to Insert ?
}

schema_end ex_schema # 
#
script
{
    if $name == "AAA" {
        $ = $ * 2 + 1; 
    }   
}
script_end ex_script
#
run 
{
    %now = 0;
    while not END_OF_ARRAY 
    {
        ex_script(ex_schema);
        next;
    }
}
#
schema # view?
{
    "owner" : %multi_key%string,
    "trade_power" : %int
}
schema_end  new_schema
#
script # 
{
    builtin_insert_to_multi_map(%parameter.1, $owner, $trade_power);
}
script_end  insert_multi_map_owner_trade_power
#
cd $root.[].{}."provinces"
#
global{} test_map
#
run
{
    %now = 0;
    while not END_OF_OBJECT
    {
        insert_multi_map_owner_trade_power(new_schema, test_map);
        next
    }
}
#
script
{
    let[] trade_power = %parameter.1[$name];
    if any trade_power > 30 { # any, all, not?, at_least(5/*number*/)?
        $ = $ + 10;
    }
}
script_end test
#
cd ..
cd "countries"
#
run 
{
    %now = 0;
    while not END_OF_ARRAY
    { 
        test(ex_schema, test_map); # before called cd(~~);
        next;
    }
}
#


# 2024-08-24
# 0. json-path?
$root
$root.[].{}."countries"
# 1. chk my? json-schema?
schema = {
    "name" : "$name%string", # condition.
    "power" : {
        "army" : "$%integer" 
    }
}
# 2. script.
script = {
    if $name == "AAA" { 
        let x <- $ * 2
        $ <- x + 1      # to update...
    }
}   
# 3. work
work = {
    name = test
    parameter = [ ]
    schema = {
        "name" : "$name%string", 
        "power" : {
            "army" : "$%integer" # case no need air, no need navy.
        }
    }    
    script = {
        if $name == "AAA" { # all comparison is cond.
            $ <- $ * 2 + 1  # to update...
        }   
    }
}
# 4. shell? 
load('citylots.json', 0); # (0 - use most cpu`s core?)
cd('json-path'); # json-path <- $root.[].{}."countries"
while $NOW != EOA /* END OF ARRAY */ {
    work('test', $NOW);
    $NOW = next($NOW);
}
# 5. make (multi-)set?, (multi-)map.
let test_map = make_multi_map($root.[].{}."provinces".$all, "owner", "trade_power");
                    #            route,                key      data
work('test2', $NOW, test_map); # before called cd(~~);

# test2`s script?
script = {
    let trade_power[] = %parameter.test_map[$name];
    if any trade_power > 30 { # any, all, not?, atleast(5/*number*/)?
        $army = $army + 10;
    }
}
# all type is pointer? 
# log...(print())
# revisit-work
work = {
    name = test1
    parameter = [ ]
    run = {
        schema = { ~~ }
        script = { ~~ }
    }
    run = {
        schema = { ~~ }
        script = { ~~ }
    }
}
work = {
    name = test2
    parameter = [ test_map ]
    run = {
        schema = { ~~ }
        script = { ~~ }
    }
    run = {
        schema = { ~~ }
        script = { ~~ }
    }
}
# parallel? 
cd(~~);
let x = make_thread('test1', $NOW);
cd(~~);
let y = make_thread('test2', $NOW);
wait(x);
wait(y);
# default variable - const, only can change in work.run.script ?
# schema - not existing key?
schema = {
    "new_key" : "$new_element%NONE" # not existing key (or value?).
}

# 2022-06-05

# make (multi-)set, (multi-)map, ...
make_map = { # using parallel stable sort? + binary search...
    name = test_map
    
    where = [ $root [] {} "provinces" ]
    
    json = {
        "owner" %str%key $owner
        "pop" %int%data $pop
    }
}

# script
script = {
    if $name == "AAA" { 
        $ = $ * 2      # to update...
    }
    
    let arr[] = test_map[$name].$pop
} 

# 2022-06-02

# schema
schema = { 
    "name" %str $name  
    "power" {
        "army" %int $ 
    }
    # "power" [ %int %int %int %int $ ]
} 

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
