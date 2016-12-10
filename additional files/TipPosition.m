function TipPosition(V,Mv,base)
  if(det(V) == 0)
    V = V+eye(4)*0.0001;
  endif
  K = (V^-1)*Mv;
  if(base == 0)      %base1
    K = [0 1 0 0; 0 0 1 0; 1 0 0 0; 0 0 0 1] * K;
  elseif(base == 1)  %base2
    K = [1 0 0 0; 0 0 -1 0; 0 1 0 0; 0 0 0 1] * K;
  endif
  x = [K(1,4), K(2,4), K(3,4)] 
endfunction
