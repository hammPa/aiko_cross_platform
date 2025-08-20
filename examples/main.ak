# print(-10);
# print(10.566);
# print(false);
# print("hello");
# var x = 99;
# print(x);

# var a = 90.67;
# print(a);


# var y = true;
# var z = "hello world";

# print(y);
# print(z);


# # binary
# var angka = 10 + 7 - 5 * 4 / 2;
# print(angka);
# print(10 - 4 * 6 / 2);


# # unary
# # print(+10);
# print(-x);
# print(!true);


# # tes assignment
# x = "hello world xiii";
# print(x);


# x = true;
# print(x);

# for i = 18 .. 20 {
#     print(i);
# }


# # tes looping, if dan scope
# for i = 0 .. 5 {
#     print("hello world muehehe");
#     var b = 5;
#     for j = 0 .. 5 {
#         print("j");
#     }
#     if(i == 3){
#         var m = 3;
#         print(m);
#     }
# }

# # tes scope if elif else
# x = 10;
# if(x <= 4){
#     print("x <= 4");
# }
# elif(x <= 6){
#     print("x <= 6");
# }
# elif(x <= 8){
#     print("x <= 8");
# }
# else {
#     print("x >= 9");
# }

# # error karna scopenya beda
# # print(b);


# # tes array
# var arr = [1, 2, 3];
# print(arr);


# # tes var = var + literal
# var val = 10;
# val = val + 5;
# print(val);

# var val2 = val * 5;
# print(val2);

# # tes += -= */ /=
# var assignOp = 5;
# assignOp %= 3;            # yang iini awalnya bisa skrg error parser
# print(assignOp);

# var arr2 = [15, 25, 54];
# var index = 10;
# print(arr2[0]);
# print(arr2[index]); # undefined behavior
# print(arr2);


# # tes break continue
# for i = 0 .. 10 {
#     if(i == 3){
#         continue;
#     }
#     if(i == 5){
#         break;
#     }
#     print(i);
# }




# # tes typeof
# var int = 10;
# var dbl = 10.56;
# var boolean = true;
# var string = "hello";

# print("tipe:");
# print(typeof(int));
# print(typeof(dbl));
# print(typeof(boolean));
# print(typeof(string));

# if(typeof(int) == "int"){
#     print("tipe data nya adalah integer");
# }


# # tes input
# var x = input("masukkan teks: ", "string");
# print(x);
# print(typeof(x));


# print("=== Kalkulator Sederhana ===");

# var a = input("Masukkan angka pertama: ", "double");
# var b = input("Masukkan angka kedua: ", "double");

# print("Pilih operasi: +, -, *, /, %");
# var op = input("Masukkan operator: ", "string");


# if(op == "+") {
#     var hasil = a + b;
#     print("Penjumlahan: ");
#     print(hasil);
# }
# elif(op == "-") {
#     var hasil = a - b;
#     print("Pengurangan: ");
#     print(hasil);
# }
# elif(op == "*") {
#     var hasil = a * b;
#     print("Perkalian: ");
#     print(hasil);
# }
# elif(op == "/") {
#     if(b == 0){
#         print("Error: pembagian dengan nol!");
#     }
#     else {
#         var hasil = a / b;
#         print("Pembagian: ");
#         print(hasil);
#     }
# }
# elif(op == "%") {
#     if(b == 0){
#         print("Error: modulo dengan nol!");
#     }
#     else {
#         var hasil = a % b;
#         print("Modulus: ");
#         print(hasil);
#     }
# }
# else {
#     print(op);
#     print("Operator tidak dikenal!");
# }



# # tes assign ke index array
# var assignArr = [1, 3, 5, 7];
# assignArr[0] = 10;
# print(assignArr);

# assignArr[2] = 99;
# print(assignArr);

# # assignArr[10] = 100; # error array out of bonds
# # print(assignArr[10]);

# var doubleArr = [10.5, 90.4, 65.6];
# doubleArr[0] = 99.4;
# print(doubleArr);



# # array get val
# var arrGet = [10, 20, 40];
# var arrGet1 = arrGet[0];
# print(arrGet1);


# # tes tipe data
# var integer;
# print(integer);



# # tes 1 function procedure
# fun displayX(x){
#     print(x);
# }


# displayX(5);



# # tes 2 function procedure + shadowing
# var outerVar = 10;
# var outerArr = [1, 2, 3];

# fun display(){
#     var outerVar = 20;
#     var outerArr = [9, 8, 7];
#     print(outerVar);
#     print(outerArr);
# }

# display();
# print(outerVar);
# print(outerArr);




# # tes 3 function procedure passing argument to print
# var outerVar = 10;

# fun displayPassing(outerVar){
#     print(outerVar);
# }

# displayPassing(outerVar);
# print(outerVar);




# # tes 4 function procedure passing argument to print with data type
# var outerVarInt = 10;

# fun displayPassingInt(outerVarInt: i32){
#     print(outerVarInt);
# }

# displayPassingInt(outerVarInt);
# print(outerVarInt);




# # tes 5 function return passing argument
# fun add(x, y: i32){
#     return x + y;
# }


# var result = add(10, 20);
# print(result);
# print(add(3, 5));




# # tes 6 function non int tapi tipe eksplisit
# fun displayString(string: str){
#     print(string);
# }

# displayString("hello");



# fun returnStr(){
#     return "omakkk ini fun return str";
# }

# var returnStrCall = returnStr(); # ini blm bisa
# print(returnStrCall);







# # tes variable dengan tipe data
# var varInt32: i32 = 10;
# print(varInt32);

# var varInt64: i64 = 2147483649;
# print(varInt64);

# var varFloat32: f32 = 10.5;
# print(varFloat32);

# var varFloat64: f64 = 9999.456;
# print(varFloat64);

# var varBool: bool = true;
# print(varBool);

# var varStr: str = "hello"; 
# print(varStr);

# # varInt32 = "cihuy"; # ini akan error karena variabel yang dideklarasikan tipenya sifatnya statis


# # tes variable dengan tipe data tapi tanpa initializer
# var explicitNoInitStr: str;
# print(explicitNoInitStr);

# var explicitNoInitDbl: f64;
# explicitNoInitDbl = 10.5;
# print(explicitNoInitDbl);


# var explicitNoInitBool: bool;
# explicitNoInitBool = true;
# print(explicitNoInitBool);




# # tes struct
# struct Person {
#     name: str,
#     age: i32
# };



# # var struct1; # ini akan jadi int dengan nilai default 0
# # var struct2: Person; # ini error
# # tes struct implisit
# var struct3 = Person{name: "hamm", age: 20};
# print(struct3.name);
# print(struct3.age);
# # print(struct3.width); # error karna tidak ada member


# struct3 = "woilah cik muehehe";
# print(struct3);
# # print(struct3.name); # error karna bukan struct lagi





# tes struct explisit
# var struct4: Person = Person{name: "hamm", age: 20};