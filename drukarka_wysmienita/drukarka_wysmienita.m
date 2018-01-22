if(~exist('s'))
	s = serial('/dev/ttyUSB0');
	s.BaudRate = 115200;
end

if(strcmp(s.Status, 'closed'))
	fopen(s);
end

figure(1)
view(3)

x1 = 0;
y1 = 0;
x2 = 0;
y2 = 16.5;
x3 = 14.3;
y3 = 8.25;

xc = 7;
yc = 8.25;
zc = 30;
r = 21.5;

col1 = line([x1 x1], [y1 y1], [0 60]);
col2 = line([x2 x2], [y2 y2], [0 60]);
col3 = line([x3 x3], [y3 y3], [0 60]);
srodek = line(xc,yc,zc);

z1 = zc + sqrt(r^2 -(y1-yc)^2 -(x1-xc)^2);
z2 = zc + sqrt(r^2 -(y2-yc)^2 -(x2-xc)^2);
z3 = zc + sqrt(r^2 -(y3-yc)^2 -(x3-xc)^2);

arm1 = line([x1 xc], [y1 yc], [z1 zc]);
arm2 = line([x2 xc], [y1 yc], [z2 zc]);
arm3 = line([x3 xc], [y1 yc], [z3 zc]);
set(arm1, 'XData', [x1 xc], 'Ydata', [y1 yc], 'Zdata', [z1 zc]);
set(arm2, 'XData', [x2 xc], 'Ydata', [y2 yc], 'Zdata', [z2 zc]);
set(arm3, 'XData', [x3 xc], 'Ydata', [y3 yc], 'Zdata', [z3 zc]);

kroki_x = 0;
kroki_y = 0;
kroki_z = 0;

for t = 0:0.1:50
    
    xc = 7 + sin(t);
    yc = cos(t) + 8.25;
    zc = 1 + (0.5*t);
    
    srodek = line(xc,yc,zc);
%     figure(1)
%     hold on
%     plot3(xc,yc,zc,'rx');

    z1 = zc + sqrt(r^2 -(y1-yc)^2 -(x1-xc)^2);
    z2 = zc + sqrt((r^2 -(y2-yc)^2 -(x2-xc)^2));
    z3 = zc + sqrt((r^2 -(y3-yc)^2 -(x3-xc)^2));

    set(arm1, 'XData', [x1 xc], 'Ydata', [y1 yc], 'Zdata', [z1 zc]);
    set(arm2, 'XData', [x2 xc], 'Ydata', [y2 yc], 'Zdata', [z2 zc]);
    set(arm3, 'XData', [x3 xc], 'Ydata', [y3 yc], 'Zdata', [z3 zc]);
    
    if(exist('z01'))
        kroki_x = (z1 - z01)*1000;
        kroki_y = (z2 - z02)*1000;
        kroki_z = (z3 - z03)*1000;
    end
    
    z01 = z1;
    z02 = z2;
    z03 = z3;
    
     tekst = ['<' num2str(round(kroki_x)) ',' num2str(round(kroki_y)) ',' num2str(round(kroki_z)) '>']
     fprintf(s,tekst);

     while(~fscanf(s))
     end
     
     pause(0.000001);
     
end

if(strcmp(s.Status, 'open'))
	fclose(s);
end