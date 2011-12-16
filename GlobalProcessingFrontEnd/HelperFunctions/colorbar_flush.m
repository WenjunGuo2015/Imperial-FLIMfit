function im=colorbar_flush(h,hc,data,mask,lim,cscale,t,intensity,int_lim)

    m = 2^16;
    w = 10;
    
    if nargin < 7
        t = '';
    end
    
    if nargin < 8
        merge = false;
    else
        merge = true;
    end

    h1 = 256;
    cbar = (h1:-1:1)'/h1;
    cbar = repmat(cbar,[1,w]);
    cbar = int32(cbar * m);
        
    % scale to lie between 1-2^16 
    data = (data - lim(1))/(lim(2)-lim(1));
    nan_mask = isnan(data);

    if merge
    
        ibar = (1:w)/w;
        ibar = repmat(ibar,[h1,1]);
        
        data(data < 0) = 0;
        data(data > 1) = 1;  
        data = int32(data * 2^16 + 1);

        cmap = cscale(m);
        cbar = ind2rgb(cbar,cmap);
        mapped_data = ind2rgb(data,cmap);

        intensity(intensity>int_lim(2)) = int_lim(2);
        intensity = intensity - int_lim(1);
        intensity(intensity<0) = 0;
        intensity = intensity / (int_lim(2)-int_lim(1));

        cbar = cbar .* repmat(ibar,[1 1 3]);
        mapped_data = mapped_data .* repmat(intensity, [1 1 3]);
        
    else
               
        
        data = int32(data * m);

        data(data < 0) = -2; % out of range below -> dark gray
        data(data > m) = -1; % out of range above -> light gray
        data(nan_mask) = -3; % failed to fit -> white

        if ~isempty(mask)
            data(mask==1) = -4; % masked -> black
        end
        
        data = data + 4;

        cmap = cscale(m);
        cmap = [ [0,0,0]; [1,1,1]; [0.33,0.33,0.33]; [0.66,0.66,0.66]; cmap];

        cbar = ind2rgb(cbar,cmap);
        mapped_data = ind2rgb(data,cmap);

    end
        
    im(1)=image(mapped_data,'Parent',h);
        
    im(2)=image(cbar,'Parent',hc);

    set(h,'XTick',[],'YTick',[]);
    set(hc,'XTick',[],'YTick',[]);
 
    pos=plotboxpos(h);
    
    set(h,'Units','pixels');
        
    
    ht1=text(pos(3), 2, num2str(lim(1)), 'Units','pixels','Parent',h,...
         'Color','w','BackgroundColor','k','Margin',1,...
         'FontUnits','points','FontSize',10,...
         'HorizontalAlignment','right','VerticalAlignment','bottom');

    ht2=text(pos(3), pos(4), num2str(lim(2)), 'Units','pixels','Parent',h,...
         'Color','w','BackgroundColor','k','Margin',1,...
         'FontUnits','points','FontSize',10,...
         'HorizontalAlignment','right','VerticalAlignment','top');

    set(ht1,'Units','normalized');
    set(ht2,'Units','normalized');
    
    if ~strcmp(t,'')
        ht3=text(3, pos(4), t, 'Units','pixels','Parent',h,...
         'Color','w','BackgroundColor','k','Margin',1,...
         'FontUnits','points','FontSize',10,...
         'HorizontalAlignment','left','VerticalAlignment','top');
        set(ht3,'Units','normalized');
    end
    
    set(h,'Units','normalized');

end