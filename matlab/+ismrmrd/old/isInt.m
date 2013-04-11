function b = isInt(a)

    b = isa(a,'integer') || (imag(a)==0 && mod(a,1)==0);

end