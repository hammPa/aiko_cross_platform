print ('saya baru belajar selama 1 bulan saya afk');
print(10 + 4 * 6 / 2);

var a = 10;
print(a);

if(a >= 10){
    print("angka > 10");
}


for i = 0 .. 5 {
    print(i);
}


var teks = "hello world";
print(teks);


fun add(f, s){
    return f +  s;
}

fun five(){
    return 5;
}

fun varA(pA){
    return pA;
}

print(add(4, 6));
print(five());
print(varA(a));


fun displayHello(){
    print("hello muehehe");
}

displayHello();

print("input pertama:");
var input1 = input();
print(input1);

if(input1 == 10){ # pengecekan hasil input dengan string error
    print("angka 10");
}
else {
    print("entah");
}


print(typeof input1 == "string");
if(typeof input1 == "string"){
    print("makakakaka");
}