function b = isInt(a)

    b = isa(a,'integer');
    if ~b && isnumeric(a)
        b =  (imag(a)==0 && mod(a,1)==0);
    end

end