function imageId = mat2omeroImage(factory, data, pixeltype, imageName, description, channels_names, modulo)

imageId = [];

            if isempty(factory) ||  isempty(data) || isempty(imageName)
                errordlg('upload_Image: bad input');
                return;
            end;                   
            %
            [sizeM,sizeX,sizeY] = size(data);
            %
            sizeC = 1;
            sizeZ = 1;
            sizeT = 1;            
            %
            switch modulo
                case 'ModuloAlongC'
                    sizeC = sizeM;
                case 'ModuloAlongZ'
                    sizeZ = sizeM;
                case 'ModuloAlongT'
                    sizeT = sizeM;
                otherwise
                    errordlg('wrong modulo specification'), return;
            end

queryService = factory.getQueryService();
pixelsService = factory.getPixelsService();
rawPixelsStore = factory.createRawPixelsStore(); 
containerService = factory.getContainerService();

% Lookup the appropriate PixelsType, depending on the type of data you have:
p = omero.sys.ParametersI();
p.add('type',rstring(pixeltype));       

q=['from PixelsType as p where p.value= :type'];
pixelsType = queryService.findByQuery(q,p);

% Use the PixelsService to create a new image of the correct dimensions:
iId = pixelsService.createImage(sizeX, sizeY, sizeZ, sizeT, toJavaList([uint32(0:(sizeC - 1))]), pixelsType, char(imageName), char(description));
imageId = iId.getValue();

% Then you have to get the PixelsId from that image, to initialise the rawPixelsStore. I use the containerService to give me the Image with pixels loaded:
image = containerService.getImages('Image',  toJavaList(uint64(imageId)),[]).get(0);
pixels = image.getPrimaryPixels();
pixelsId = pixels.getId().getValue();
rawPixelsStore.setPixelsId(pixelsId, true);

minVal = min(data(:));
maxVal = max(data(:));

for m = 1:sizeM % t is plane number    
    plane = squeeze(data(m,:,:));    
    bytear=omerojava.util.GatewayUtils.convertClientToServer(pixels, plane) ;
        switch modulo
            case 'ModuloAlongC'
                rawPixelsStore.setPlane(bytear, int32(0),int32(m-1),int32(0));                
                pixelsService.setChannelGlobalMinMax(pixelsId, m-1, minVal, maxVal);                
            case 'ModuloAlongZ'
                rawPixelsStore.setPlane(bytear, int32(m-1),int32(0),int32(0));        
            case 'ModuloAlongT'
                rawPixelsStore.setPlane(bytear, int32(0),int32(0),int32(m-1));        
        end        
end 
%
if ~strcmp(modulo,'ModuloAlongC')
    pixelsService.setChannelGlobalMinMax(pixelsId, 0, minVal, maxVal);                
else %set channels names
if nargin == 7 && ~isempty(channels_names) && sizeC == numel(channels_names)
    %
    pixelsDesc = pixelsService.retrievePixDescription(pixels.getId().getValue());
    channels = pixelsDesc.copyChannels();
    %         
    for c = 1:sizeC
        ch = channels.get(c - 1);
        ch.getLogicalChannel().setName(omero.rtypes.rstring(char(channels_names{c})));
        factory.getUpdateService().saveAndReturnObject(ch.getLogicalChannel());
    end                                                        
end;
    
end
%
rawPixelsStore.save();
rawPixelsStore.close();
%
RENDER = true;

re = factory.createRenderingEngine();
%
re.lookupPixels(pixelsId)
    if ~re.lookupRenderingDef(pixelsId)
        re.resetDefaults();  
    end;
    if ~re.lookupRenderingDef(pixelsId)
        errordlg('mat2omeroImage: can not render properly');
        RENDER = false;
    end
%
if RENDER
    try
        % start the rendering engine
        re.load();
        % optional setting of rendering 'window' (levels)
        %renderingEngine.setChannelWindow(cIndex, float(minValue), float(maxValue))
        %
        alpha = 255;
        if strcmp(modulo,'ModuloAlongC')
            if 3==sizeC % likely RGB
                    re.setRGBA(0, 255, 0, 0, alpha);
                    re.setRGBA(1, 0, 255, 0, alpha);
                    re.setRGBA(2, 0, 0, 255, alpha);
            else
                    for c = 1:sizeC,
                        re.setRGBA(c - 1, 255, 255, 255, alpha);
                    end
            end            
        else
            re.setRGBA(0, 255, 255, 255, alpha);
        end
        %
        re.saveCurrentSettings();
    catch err
        [ST,~] = dbstack('-completenames'); errordlg([err.message ' in the function ' ST.name],'Error');
    end
end;

re.close();