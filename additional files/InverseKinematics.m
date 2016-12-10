function InverseKinematics(xd,q_0,typeID) 
  %t = linspace (0, 1000, 1000);
  t = logspace(0,3,120);
  if(typeID == 0)     % dwuwahadlo planarne
    d1 = 6.79487;
    d2 = 9.65503;
    J =  @(q) [-sind(q(1))*d1 - sind(q(1)+q(2))*d2, -sind(q(1)+q(2))*d2;
                cosd(q(1))*d1 + cosd(q(1)+q(2))*d2, cosd(q(1)+q(2))*d2];
    %handler = @(q) 0.5*[-sind(q(1))*cosd(q(2))*d2-cosd(q(1))*sind(q(2))*d2-sind(q(1))*d1, -cosd(q(1))*sind(q(2))*d2-sind(q(1))*cosd(q(2))*d2; cosd(q(1))*cosd(q(2))*d2-sind(q(1))*sind(q(2))*d2+cosd(q(1))*d1, -sind(q(1))*sind(q(2))*d2+cosd(q(1))*cosd(q(2))*d2]^-1 * (xd - [cosd(q(1))*cosd(q(2))*d2 - sind(q(1))*sind(q(2))*d2 + cosd(q(1))*d1; sind(q(1))*cosd(q(2))*d2 + cosd(q(1))*sind(q(2))*d2 + sind(q(1))*d1]);
    handler = @(q) 0.5 * J(q)^-1 * (xd - [cosd(q(1))*d1 + cosd(q(1)+q(2))*d2; sind(q(1))*d1 + sind(q(1)+q(2))*d2]);

  elseif(typeID == 1) % trojwahadlo planarne
    d1 = 6.79487;
    d2 = 7.79483;
    d3 = 6.655;
    J =  @(q) [-sind(q(1))*d1 - sind(q(1)+q(2))*d2 - sind(q(1)+q(2)+q(3))*d3, -sind(q(1)+q(2))*d2 - sind(q(1)+q(2)+q(3))*d3, - sind(q(1)+q(2)+q(3))*d3;
               cosd(q(1))*d1 + cosd(q(1)+q(2))*d2 + cosd(q(1)+q(2)+q(3))*d3, cosd(q(1)+q(2))*d2 + cosd(q(1)+q(2)+q(3))*d3, cosd(q(1)+q(2)+q(3))*d3];
    handler = @(q) 0.5 * J(q)' * (J(q) * J(q)')^-1 * (xd - [cosd(q(1))*d1 + cosd(q(1)+q(2))*d2 + cosd(q(1)+q(2)+q(3))*d3; sind(q(1))*d1 + sind(q(1)+q(2))*d2 + sind(q(1)+q(2)+q(3))*d3]);
  elseif(typeID == 2 ) % SCARA
    d1 = 9.61;
    d4 = 3.7;
    a1 = 6.63987;
    a2 = 6.15503;
    J = @(q) [-a2*sind(q(1)+q(2))-a1*sind(q(1)), -a2*sind(q(1)+q(2)), 0, 0;
              a2*cosd(q(1)+q(2))+a1*cosd(q(1)), a2*cosd(q(1)+q(2)), 0, 0;
              0, 0, 0, -1];
    handler = @(q) 0.5 * J(q)' * (J(q) * J(q)')^-1 * (xd - [a2*cosd(q(1)+q(2))+a1*cosd(q(1));  a2*sind(q(1)+q(2))+a1*sind(q(1)); -q(4)-d4+d1]);     
    
  elseif(typeID == 3 ) % fanuc
    a1 = 2;
    a2 = 8.79483;
    a3 = 2.155;
    d1 = 7.24987;
    d4 = 7.18987;
    d6 = 7.70503;
    J = @(q) [-d6*sind(q(1))*sind(q(2)+q(3))*sind(q(4))*sind(q(5))+d6*cosd(q(1))*cosd(q(4))*sind(q(5))-d6*sind(q(1))*cosd(q(2)+q(3))*cosd(q(5))-d4*sind(q(1))*cosd(q(2)+q(3))+a3*sind(q(1))*sind(q(2)+q(3))+a2*sind(q(1))*sind(q(2))-a1*sind(q(1)), d6*cosd(q(1))*cosd(q(2)+q(3))*sind(q(4))*sind(q(5))-d6*cosd(q(1))*sind(q(2)+q(3))*cosd(q(5))-d4*cosd(q(1))*sind(q(2)+q(3))-a3*cosd(q(1))*cosd(q(2)+q(3))-a2*cosd(q(1))*cosd(q(2)), d6*cosd(q(1))*cosd(q(2)+q(3))*sind(q(4))*sind(q(5))-d6*cosd(q(1))*sind(q(2)+q(3))*cosd(q(5))-d4*cosd(q(1))*sind(q(2)+q(3))-a3*cosd(q(1))*cosd(q(2)+q(3)), d6*cosd(q(1))*sind(q(2)+q(3))*cosd(q(4))*sind(q(5))-d6*sind(q(1))*sind(q(4))*sind(q(5)), d6*cosd(q(1))*sind(q(2)+q(3))*sind(q(4))*cosd(q(5))+d6*sind(q(1))*cosd(q(4))*cosd(q(5))-d6*cosd(q(1))*cosd(q(2)+q(3))*sind(q(5));
              d6*cosd(q(1))*sind(q(2)+q(3))*sind(q(4))*sind(q(5))+d6*sind(q(1))*cosd(q(4))*sind(q(5))+d6*cosd(q(1))*cosd(q(2)+q(3))*cosd(q(5))+d4*cosd(q(1))*cosd(q(2)+q(3))-a3*cosd(q(1))*sind(q(2)+q(3))-a2*cosd(q(1))*sind(q(2))+a1*cosd(q(1)), d6*sind(q(1))*cosd(q(2)+q(3))*sind(q(4))*sind(q(5))-d6*sind(q(1))*sind(q(2)+q(3))*cosd(q(5))-d4*sind(q(1))*sind(q(2)+q(3))-a3*sind(q(1))*cosd(q(2)+q(3))-a2*sind(q(1))*cosd(q(2)), d6*sind(q(1))*cosd(q(2)+q(3))*sind(q(4))*sind(q(5))-d6*sind(q(1))*sind(q(2)+q(3))*cosd(q(5))-d4*sind(q(1))*sind(q(2)+q(3))-a3*sind(q(1))*cosd(q(2)+q(3)), d6*sind(q(1))*sind(q(2)+q(3))*cosd(q(4))*sind(q(5))+d6*cosd(q(1))*sind(q(4))*sind(q(5)), d6*sind(q(1))*sind(q(2)+q(3))*sind(q(4))*cosd(q(5))-d6*cosd(q(1))*cosd(q(4))*cosd(q(5))-d6*sind(q(1))*cosd(q(2)+q(3))*sind(q(5));
              0, d6*sind(q(2)+q(3))*sind(q(4))*sind(q(5))+d6*cosd(q(2)+q(3))*cosd(q(5))+d4*cosd(q(2)+q(3))-a3*sind(q(2)+q(3))-a2*sind(q(2)), d6*sind(q(2)+q(3))*sind(q(4))*sind(q(5))+d6*cosd(q(2)+q(3))*cosd(q(5))+d4*cosd(q(2)+q(3))-a3*sind(q(2)+q(3)), -d6*cosd(q(2)+q(3))*cosd(q(4))*sind(q(5)), -d6*cosd(q(2)+q(3))*sind(q(4))*cosd(q(5))-d6*sind(q(2)+q(3))*sind(q(5))];
    k = @(q) [d6*cosd(q(1))*sind(q(2)+q(3))*sind(q(4))*sind(q(5))+d6*sind(q(1))*cosd(q(4))*sind(q(5))+d6*cosd(q(1))*cosd(q(2)+q(3))*cosd(q(5))+d4*cosd(q(1))*cosd(q(2)+q(3))-a3*cosd(q(1))*sind(q(2)+q(3))-a2*cosd(q(1))*sind(q(2))+a1*cosd(q(1));
              d6*sind(q(1))*sind(q(2)+q(3))*sind(q(4))*sind(q(5))-d6*cosd(q(1))*cosd(q(4))*sind(q(5))+d6*sind(q(1))*cosd(q(2)+q(3))*cosd(q(5))+d4*sind(q(1))*cosd(q(2)+q(3))-a3*sind(q(1))*sind(q(2)+q(3))-a2*sind(q(1))*sind(q(2))+a1*sind(q(1));
              -d6*cosd(q(2)+q(3))*sind(q(4))*sind(q(5))+d6*sind(q(2)+q(3))*cosd(q(5))+d4*sind(q(2)+q(3))+a3*cosd(q(2)+q(3))+a2*cosd(q(2))+d1];
    handler = @(q)  0.5 * J(q)' * (J(q) * J(q)')^-1 * (xd - k(q));
  endif
  qd = lsode(handler,q_0,t)
  %qf = qd(end,:)
endfunction