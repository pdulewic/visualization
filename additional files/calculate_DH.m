## użycie: calculate_DH(Z,A)
## 
## Funckja implementuje algorytm Denavita-Hartenberga. Macierz 'Z' zawiera 
## wektory definiujące kolejne osie 'z'. 'A' jest tablicą komórek, przechowującą
## macierze 4x4, za pomocą których można odwzorować układy współrzędnych
## kolejnych wektorów do bazowego układu współrzednych. W ten sposób wszystkie
## osie 'z' są wyrażnowe w tym samym układzie współrzędnych. 

function calculate_DH(Z,A)
  n = size(A)(2);  %liczba układów współrzędnych
  
  % Algorytm działa przy założeniu, że mamy co najmniej 2 układy współrzędnych.
  % W innym wypadku tabela DH byłaby pusta. 
  if (n < 2)      
    printf ('TOOSMALL\n'); % kod błędu dla wywołującego algorytm programu
    return
  endif
  
  % Bazowy układ współrzędnych nie powinien być poddany żadnej transformacji, 
  % czyli macierz A{1} powinna być macierzą jednostkową. Jeśli tak nie jest, 
  % algorytm przekształca wszystkie układy tak, żeby warunek ten został 
  % spełniony. 
  if(A{1} != eye(4))
    for i = (2 : n)
      A{i} = A{1}^-1 * A{i};
    endfor
    A{1} = eye(4);
  endif
  
  % utworzenie 3 macierzy zawierających finalne układy współrzędnych
  % macierz punktów, stanowiących początki układów współrzędnych
  origins = zeros(n,3); 
  % macierz punktów wyznaczających osie 'x' 
  x_axis = zeros(n,3);
  % macierz punktów wyznaczających osie 'z'
  z_axis = zeros(n,3);
  
  % Początek pierwszego i ostatniego układu współrzędnych są zawsze narzucone
  origins(1,:) = A{1}(1:3,4)';
  origins(n,:) = A{n}(1:3,4)';
  
  % wyznaczenie z0 i x0
  z_axis(1,:) = (A{1} * [Z(1,:) , 1]')'(1:3) - origins(1,:);
  % zakłada się, że z0 może przyjnować wartości [1, 0, 0] lub [0, 1, 0]
  x_axis(1,:) = [z_axis(1,2) z_axis(1,1) 0];
  
  % Wyznaczenie układów współrzędnych
  for i = (2 : n-1) 
    % oś 'z' o indeksie i-1 jest już wyliczona
    p1 = origins(i-1,:)';
    z1 = z_axis(i-1,:)';
    % i-ta oś 'z' musi zostać obliczona na podstawie 'Z' oraz 'A'
    p2 = A{i}(1:3,4);
    z2 = (A{i} * [Z(i,:) , 1]')(1:3) - p2;
      
    % Rozwiązanie równania, którego wynikiem są punkty przecięcia osi 'z'. Jeśli
    % osie się nie przecinają, rozwiązaniem będą punkty leżące najbliżej siebie
    % na obu prostych. 
    M = [z1 , -z2];
    b = (p2 - p1);
    x = M\b;
    
    % korekcja błędów numerycznych przy równoległych osiach x
    if(abs(x(1)) > 10000)
      x(1) = 0;
    endif
    if(abs(x(2)) > 10000)
      x(2) = 0;
    endif
    
    % Punkty leżące najbliżej siebie na obu prostych. Jesli N1 == N2, to proste
    % przecinają się.
    N1 = p1 + x(1) * z1;
    N2 = p2 + x(2) * z2;
    
    % Sprawdzenie, czy osie 'z' nie nachodzą na siebie
    cr1 = cross(z1, p2 - p1);
    cr2 = cross(z1, p2 + z2 - p1);
    
    % Wyznaczenie poczatków układów wspołrzędnych, i osi 'x'.
    % Z powodu błędów numerycznych wartości różniące się o 0.001 od zera są
    % traktowane tak samo jak zero. 
    if(norm(cr1) < 0.001 & norm(cr2) < 0.001)  % osie 'z' nachodzą na siebie
      origins(i,:) = origins(i-1,:);   % początek układu i oś x są takie same 
      x_axis(i,:) = x_axis(i-1,:);     % jak dla poprzedniej iteracji 
    elseif(norm(N1 - N2) < 0.001)  % z(i-1) przecina zi
      x_axis(i,:) = (cross(z1, z2))';  % x = z(i-1) x zi
      origins(i,:) = N2'; 
    else                           % osie z nie przecinają się
      origins(i,:) = N2';
      if(norm(cross(z1,z2)) < 0.001) % osie z są równoległe
        % wyznaczenie normalnej z(i-1), zi przecinającej zi w punkcie N2
        par = (z1(1)*(N2(1) - p1(1)) + z1(2)*(N2(2) - p1(2)) + z1(3)*(N2(3) - p1(3)))/(z1(1)^2 + z1(2)^2 + z1(3)^2);
        N1 = p1 + par * z1;  
      endif
      x_axis(i,:) = (N2 - N1)';        % oś 'x' na przedłużeniu normalnej
    endif
      z_axis(i,:) = z2';  % dopełnienie układu o oś 'z'
  endfor
  
  % Wyznaczenie układu współrzędnych efektora
  z_axis(n,:) = z_axis(n-1,:);                % zn równoległe do z(n-1)
  y_n = (A{n}*[0 0 1 1]')'(1:3)-origins(n,:); % yn w kierunku zamykania chwytaka
  x_axis(n,:) = cross(y_n , z_axis(n,:));     % xn dookreślone do układu 
                                              % prawoskrętnego
  
  % Wektory definiujące osie x powstają w wyniku iloczynu wektorowego, mogą więc
  % mieć różne długości i należy je znormalizować  
  for i = (1 : n-1)
    x_axis(i,:) = x_axis(i,:)/norm(x_axis(i,:));
  endfor
  
  % Wyznaczenie parametrów Denavita-Hartenberga
  DH_table = zeros(n-1,4);
  for i = (2 : n)  
    DH_index = [1,4]; % indeksy kolumn liczonych kątów w tabeli DH
    for j = (1 : 2)
      if(j == 1)  % obrót wokół osi z
        rot_axis = z_axis(i-1,:); % wektor wyznaczający oś obrotu
        w0 = x_axis(i-1,:);       % wektor początkowy
        w1 = x_axis(i,:);         % wektor końcowy
      else        % obrót wokół osi x
        rot_axis = x_axis(i,:);
        w0 = z_axis(i-1,:);
        w1 = z_axis(i,:);
      endif
      % Obliczenie cosinusa kąta pomiędzy wektorami w0 oraz w1
      cos_angle = (w0*w1') / (norm(w0) * norm(w1));
      % ze względu na błędy numeryczne, wynik może nieznacznie wykraczać poza 
      % przedział [-1, 1]
      if(cos_angle > 1)
        cos_angle = 1;
      elseif(cos_angle < -1)
        cos_angle = -1;
      endif
      angle = acosd( cos_angle );
      % sprawdzenie, czy kąt nie jest ujemny. 
      % Dla dodatniego kąta:     w1 x w2 = rot_axis
      % Dla ujemnego kąta:       w1 x w2 = -rot_axis
      cr = cross(w0,w1);
      % Ze względu na błędy numeryczne nie można po prostu użyć '=='
      if( norm(rot_axis - cr) > 0.0001 )
        angle = angle * -1;
      endif
      DH_table(i-1,DH_index(j)) = angle;
    endfor
    
    % obliczenie translacji wzdłuż osi 'z' i 'x'    
    z = z_axis(i-1,:);
    x = x_axis(i,:);
    translation = origins(i,:)' - origins(i-1,:)';
    tr = [z(1) x(1); z(2) x(2); z(3) x(3)]\translation;
    DH_table(i-1,2) = tr(1);
    DH_table(i-1,3) = tr(2);
  endfor
  
  % Wypisanie tabeli DH na standardowe wyjście
  printf('DH\n');
  DH_table
  % Wypisanie informacji o typie pierwszego przegubu
  if( z_axis(1,:) == [0 1 0])
    printf('base2\n');
  else
    printf('base1\n');
  endif
endfunction
