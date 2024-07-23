![tools-chain.jpg](..%2Fass%2Fday1%2Ftools-chain.jpg)
**Below are the detailed steps above how to use the building tool to export Cmake project and how to config Clion**

1- Install the Env building tool

2- Open and register the env to system

![env.png](..%2Fass%2Fday1%2Fenv.png)

3- Config success

![success.jpg](..%2Fass%2Fday1%2Fsuccess.jpg)

4- Enter the bsp folder

![bsp.jpg](..%2Fass%2Fday1%2Fbsp.jpg)

5- Right click mouse and select ConeMu here

6- Run the command `scons --dist` to generate project

7- `cd ./dist/project` and run the command `scons --target=cmake` To generate Cmakelist.

8- Open Clion and open the project

9- Config tool-chain
![tools-chain.jpg](..%2Fass%2Fday1%2Ftools-chain.jpg)

10- Config Open-OCD
![open-ocd.jpg](..%2Fass%2Fday1%2Fopen-ocd.jpg)

11- As the burning tool is dap-link so the config content is

```
adapter driver cmsis-dap

transport select swd

source [find target/stm32f4x.cfg]

adapter speed 10000

```

12- Compile and burning

![OPENOCD.jpg](..%2Fass%2Fday1%2FOPENOCD.jpg)