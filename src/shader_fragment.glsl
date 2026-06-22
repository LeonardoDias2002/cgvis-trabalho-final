#version 330 core

// Atributos de fragmentos recebidos como entrada ("in") pelo Fragment Shader.
// Neste exemplo, este atributo foi gerado pelo rasterizador como a
// interpolação da posição global e a normal de cada vértice, definidas em
// "shader_vertex.glsl" e "main.cpp".
in vec4 position_world;
in vec4 normal;

// Posição do vértice atual no sistema de coordenadas local do modelo.
in vec4 position_model;

// Coordenadas de textura obtidas do arquivo OBJ (se existirem!)
in vec2 texcoords;

//luz pontual
uniform vec3 g_PosLuz;

// Matrizes computadas no código C++ e enviadas para a GPU
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

// Identificador que define qual objeto está sendo desenhado no momento
#define SPHERE 0
#define BUNNY  1
#define PLANE  2
#define TACO   3
#define BOLA   4
#define BURACO 5
#define TRAJETORIA 6
#define BANDEIRA 7
#define MASTRO 8
#define HUD_BARRA 9
#define GRAMA 10
#define PISTALOOP 11

#define PISTA_CHAO 13
#define PISTA_PAREDE 14
#define ARVORE_ALTA 15
#define ARVORE_BAIXA 16
#define TRONCO 23
#define CACTUS 17
#define PISTACURVA 18
#define PISTASIMPLES 19
#define BORDASSIMPLES 20
#define BORDASCURVA 21
#define BORDASLOOP 22
#define ZEPPELIN 24


uniform int object_id;
uniform float u_ForcaPercent; // Usado para a barra de força
uniform int u_TexturaGramaPista;   // 0=rocky, 1=brick, 2=solid green
uniform int u_TexturaParedesPista; // 0=rocky, 1=brick, 2=solid gray
uniform int u_TexturaBola;         // 0=white, 1=brick, 2=rocky
uniform int u_TexturaTaco;         // 0=metal, 1=textured, 2=brick
uniform vec3 u_TrailColor;         // Cor da trilha (RGB)
uniform float u_TrailOpacity;      // Opacidade da trilha (0.0 a 1.0)
uniform vec3 u_HolePosition;       // Posicao do buraco no mundo
uniform int u_BolaInativa;
uniform float u_Time;

// Parâmetros da axis-aligned bounding box (AABB) do modelo
uniform vec4 bbox_min;
uniform vec4 bbox_max;

// Variáveis para acesso das imagens de textura
uniform sampler2D TextureImage0;
uniform sampler2D TextureImage1;
uniform sampler2D TextureImage2;
uniform sampler2D TextureImage3; // Folhas (t7.jpg)
uniform sampler2D TextureImage4; // Tronco (tronco.jpg)
uniform sampler2D TextureImage5; // Grama (grass.jpg)
uniform sampler2D TextureImage6; // Track (track.jpg)
uniform sampler2D TextureImage7; // Zeppelin metal
uniform sampler2D TextureImage8; // Logo (GOLFinho)
uniform sampler2D TextureImage9; // paredes padrão
// O valor de saída ("out") de um Fragment Shader é a cor final do fragmento.
out vec4 color;

// Constantes
#define M_PI   3.14159265358979323846
#define M_PI_2 1.57079632679489661923

void main()
{
    // Obtemos a posição da câmera utilizando a inversa da matriz que define o
    // sistema de coordenadas da câmera.
    vec4 origin = vec4(0.0, 0.0, 0.0, 1.0);
    vec4 camera_position = inverse(view) * origin;

    // O fragmento atual é coberto por um ponto que percente à superfície de um
    // dos objetos virtuais da cena. Este ponto, p, possui uma posição no
    // sistema de coordenadas global (World coordinates). Esta posição é obtida
    // através da interpolação, feita pelo rasterizador, da posição de cada
    // vértice.
    vec4 p = position_world;

    // Normal do fragmento atual, interpolada pelo rasterizador a partir das
    // normais de cada vértice.
    vec3 n = normalize(normal.xyz);

    // Vetor que define o sentido da fonte de luz em relação ao ponto atual.
    vec3 l = normalize(g_PosLuz - p.xyz);

    // Vetor que define o sentido da câmera em relação ao ponto atual.
    vec3 v = normalize(camera_position.xyz - p.xyz);

    // Coordenadas de textura U e V
    float U = 0.0;
    float V = 0.0;

	// Coeficiente de refletância difusa
    vec3 Kd0 = vec3(0.5, 0.5, 0.5);
    vec3 Ks = vec3(0.0, 0.0, 0.0);
    float q = 1.0;

    vec4 position_world = model * position_model;

    if ( object_id == SPHERE )
    {
        // PREENCHA AQUI as coordenadas de textura da esfera, computadas com
        // projeção esférica EM COORDENADAS DO MODELO. Utilize como referência
        // o slides 134-150 do documento Aula_20_Mapeamento_de_Texturas.pdf.
        // A esfera que define a projeção deve estar centrada na posição
        // "bbox_center" definida abaixo.

        // Você deve utilizar:
        //   função 'length( )' : comprimento Euclidiano de um vetor
        //   função 'atan( , )' : arcotangente. Veja https://en.wikipedia.org/wiki/Atan2.
        //   função 'asin( )'   : seno inverso.
        //   constante M_PI
        //   variável position_model

        vec4 bbox_center = (bbox_min + bbox_max) / 2.0;
        vec4 d = position_model - bbox_center;

        float rho   = length(d);
        float theta = atan(d.x,d.z);
        float phi   = asin(d.y / rho);

        U = (theta + M_PI) / 2.0 / M_PI;
        V = (phi + M_PI_2) / M_PI;

		// Obtemos a refletância difusa a partir da leitura da imagem TextureImage0
		Kd0 = texture(TextureImage0, vec2(U,V)).rgb;
    }
    else if ( object_id == BUNNY )
    {
        // PREENCHA AQUI as coordenadas de textura do coelho, computadas com
        // projeção planar XY em COORDENADAS DO MODELO. Utilize como referência
        // o slides 99-104 do documento Aula_20_Mapeamento_de_Texturas.pdf,
        // e também use as variáveis min*/max* definidas abaixo para normalizar
        // as coordenadas de textura U e V dentro do intervalo [0,1]. Para
        // tanto, veja por exemplo o mapeamento da variável 'p_v' utilizando
        // 'h' no slides 158-160 do documento Aula_20_Mapeamento_de_Texturas.pdf.
        // Veja também a Questão 4 do Questionário 4 no Moodle.

        float minx = bbox_min.x;
        float maxx = bbox_max.x;

        float miny = bbox_min.y;
        float maxy = bbox_max.y;

        float minz = bbox_min.z;
        float maxz = bbox_max.z;

        U = (position_model.x - minx) / (maxx - minx);
        V = (position_model.y - miny) / (maxy - miny);

		// Obtemos a refletância difusa a partir da leitura da imagem TextureImage0
		Kd0 = texture(TextureImage0, vec2(U,V)).rgb;
    }
    else if ( object_id == PLANE )
    {
        // Coordenadas de textura do plano, obtidas do arquivo OBJ.
        U = texcoords.x;
        V = texcoords.y;

		// Obtemos a refletância difusa a partir da leitura da imagem TextureImage1
		Kd0 = texture(TextureImage1, vec2(U,V)).rgb;
    }
    else if ( object_id == TACO )
    {
        if (u_TexturaTaco == 0) {
            Kd0 = vec3(0.7, 0.7, 0.7); // taco de metal (padrao)
        } else if (u_TexturaTaco == 1) {
            U = texcoords.x; V = texcoords.y;
            Kd0 = texture(TextureImage2, vec2(U,V)).rgb; // textura golf club
        } else {
            U = texcoords.x; V = texcoords.y;
            Kd0 = texture(TextureImage0, vec2(U,V)).rgb; // tijolo
        }
        Ks = vec3(0.8, 0.8, 0.8);
        q = 32.0;
    }
    else if ( object_id == BOLA )
    {
        vec4 bc = (bbox_min + bbox_max) / 2.0;
        vec4 bd = position_model - bc;
        float brho = length(bd);
        float btheta = atan(bd.x, bd.z);
        float bphi = asin(bd.y / brho);
        U = (btheta + M_PI) / 2.0 / M_PI;
        V = (bphi + M_PI_2) / M_PI;
        if (u_TexturaBola == 0)
            Kd0 = vec3(1.0, 1.0, 1.0); // branca (padrao)
        else if (u_TexturaBola == 1)
            Kd0 = texture(TextureImage0, vec2(U,V)).rgb; // tijolo
        else
            Kd0 = texture(TextureImage1, vec2(U,V)).rgb; // rochoso
        Ks = vec3(0.5, 0.5, 0.5);
        q = 64.0;
        
        if (u_BolaInativa == 1) {
            float ring = fract(V * 15.0 - u_Time * 3.0);
            if (ring < 0.2) {
                Kd0 = vec3(0.0, 0.8, 1.0);
                Ks = vec3(1.0, 1.0, 1.0);
            }
        }
    }
    else if ( object_id == BURACO )
    {
        if (position_model.y > 0.0) discard; // discard top half of the sphere cup
        if (camera_position.y < u_HolePosition.y - 0.05) discard; // do not render if camera is below the hole
        Kd0 = vec3(0.1, 0.1, 0.1); // buraco escurinho
        Ks = vec3(0.0, 0.0, 0.0);
        q = 1.0;
    }
    else if ( object_id == TRAJETORIA )
    {
        Kd0 = u_TrailColor;  // Usa a cor da trilha do uniform
        Ks = vec3(0.0, 0.0, 0.0);
        q = 1.0;
    }
    else if ( object_id == BANDEIRA )
    {
        // Rotaciona a textura em 90 graus e inverte U para desespelhar a logo
        U = 1.0 - texcoords.y;
        V = 1.0 - texcoords.x;
        
        // Dá um pequeno zoom na logo para ela preencher melhor a bandeira
        U = (U - 0.5) * 0.85 + 0.5;
        V = (V - 0.5) * 0.85 + 0.5;

        if (U >= 0.0 && U <= 1.0 && V >= 0.0 && V <= 1.0) {
            vec4 logo = texture(TextureImage8, vec2(U,V));
            // Interpola suavemente as bordas usando o Alpha, fica muito mais nítido de longe
            Kd0 = mix(vec3(0.02, 0.02, 0.02), logo.rgb, logo.a);
        } else {
            Kd0 = vec3(0.02, 0.02, 0.02); // Fundo preto puro para a bandeira
        }
        Ks = vec3(0.2, 0.2, 0.2); // Leve brilho
        q = 10.0;
    }
    else if ( object_id == ZEPPELIN )
    {
        // Usa a textura metálica gerada para o corpo
        U = texcoords.x * 2.0;
        V = texcoords.y * 2.0;
        vec3 zeppelinColor = texture(TextureImage7, vec2(U,V)).rgb;

        // Posição para colocar a logo na lateral do Zeppelin
        float lu = texcoords.x * 4.0 - 1.5;
        float lv = texcoords.y * 4.0 - 1.5;

        if (lu >= 0.0 && lu <= 1.0 && lv >= 0.0 && lv <= 1.0) {
            vec4 logo = texture(TextureImage8, vec2(1.0 - lv, 1.0 - lu));
            zeppelinColor = mix(zeppelinColor, logo.rgb, logo.a);
        }

        // Se quiser a logo do outro lado também
        float lu2 = texcoords.x * 4.0 - 1.5;
        float lv2 = texcoords.y * 4.0 + 0.5;
        if (lu2 >= 0.0 && lu2 <= 1.0 && lv2 >= 0.0 && lv2 <= 1.0) {
            vec4 logo = texture(TextureImage8, vec2(lv2, lu2));
            zeppelinColor = mix(zeppelinColor, logo.rgb, logo.a);
        }

        Kd0 = zeppelinColor;
        Ks = vec3(0.8, 0.8, 0.8); // Muito reflexivo (metálico)
        q = 64.0;
    }
    else if ( object_id == GRAMA )
    {
        vec2 d = position_world.xz - u_HolePosition.xz;
        if (dot(d, d) < 0.0144 && abs(position_world.y - u_HolePosition.y) < 0.5) discard;
        U = position_world.x * 0.5;
        V = position_world.z * 0.5;
        Kd0 = texture(TextureImage5, vec2(U,V)).rgb; // grama baixada
        Ks = vec3(0.0, 0.0, 0.0);
        q = 1.0;
    }
    else if ( object_id == PISTALOOP || object_id == PISTACURVA || object_id == PISTASIMPLES )
    {
        vec2 d = position_world.xz - u_HolePosition.xz;
        if (dot(d, d) < 0.0144 && abs(position_world.y - u_HolePosition.y) < 0.1) discard;
        vec4 world_pos = position_world;
        
        // Pistas são o chão. Box projection para evitar esticamento no loop:
        vec3 n_abs = abs(normalize(normal.xyz));
        if (n_abs.y > n_abs.x && n_abs.y > n_abs.z) {
            U = world_pos.x * 2.0;
            V = world_pos.z * 2.0;
        } else if (n_abs.x > n_abs.z) {
            U = world_pos.z * 2.0;
            V = world_pos.y * 2.0;
        } else {
            U = world_pos.x * 2.0;
            V = world_pos.y * 2.0;
        }

        if (u_TexturaGramaPista == 0)
            Kd0 = texture(TextureImage1, vec2(U,V)).rgb;
        else if (u_TexturaGramaPista == 1)
            Kd0 = texture(TextureImage0, vec2(U,V)).rgb;
        else if (u_TexturaGramaPista == 2)
            Kd0 = texture(TextureImage6, vec2(U,V)).rgb;
        else
            Kd0 = vec3(0.2, 0.7, 0.2);
    }
    else if ( object_id == BORDASCURVA || object_id == BORDASLOOP ||  object_id == BORDASSIMPLES)
    {
        vec4 world_pos = position_world;
        
        // Bordas são as paredes. Box projection para não esticar:
        vec3 n_abs = abs(normalize(normal.xyz));
        if (n_abs.y > n_abs.x && n_abs.y > n_abs.z) {
            U = world_pos.x * 3.0;
            V = world_pos.z * 3.0;
        } else if (n_abs.x > n_abs.z) {
            U = world_pos.z * 3.0;
            V = world_pos.y * 3.0;
        } else {
            U = world_pos.x * 3.0;
            V = world_pos.y * 3.0;
        }

        if (u_TexturaParedesPista == 0)
            Kd0 = texture(TextureImage9, vec2(U,V)).rgb;
        else if (u_TexturaParedesPista == 1)
            Kd0 = texture(TextureImage0, vec2(U,V)).rgb;
        else
            Kd0 = vec3(0.5, 0.5, 0.5);
    }
    else if ( object_id == MASTRO )
    {
        Kd0 = vec3(0.6, 0.6, 0.6); // mastro metálico
        Ks = vec3(0.5, 0.5, 0.5);
        q = 32.0;
    }
    else if ( object_id == ARVORE_ALTA || object_id == ARVORE_BAIXA )
    {
        vec4 world_pos = model * position_model;
        U = world_pos.x * 0.5;
        V = world_pos.y * 0.5;
        vec3 texture_color = texture(TextureImage3, vec2(U,V)).rgb;
        Kd0 = texture_color;
        Ks = vec3(0.0, 0.0, 0.0);
        q = 1.0;
    }
    else if ( object_id == TRONCO )
    {
        vec4 world_pos = model * position_model;
        U = world_pos.x * 0.5;
        V = world_pos.y * 0.5;
        vec3 texture_color = texture(TextureImage4, vec2(U,V)).rgb;
        Kd0 = texture_color;
        Ks = vec3(0.0, 0.0, 0.0);
        q = 1.0;
    }
    else if ( object_id == CACTUS )
    {
        Kd0 = vec3(0.2, 0.55, 0.15); // verde cactus
        Ks = vec3(0.0, 0.0, 0.0);
        q = 1.0;
    }

    if ( object_id == HUD_BARRA ) 
    {
        // Interface 2D na tela, sem iluminação.
        // Interpolação de cor baseada na força: Verde -> Amarelo -> Vermelho
        vec3 colorGreen = vec3(0.0, 1.0, 0.0);
        vec3 colorRed = vec3(1.0, 0.0, 0.0);
        // O mix funciona bem direto para verde e vermelho, criando tons quentes no meio.
        color.rgb = mix(colorGreen, colorRed, u_ForcaPercent);
        color.a = 1.0;
        return; // não aplica iluminação
    }

    // Equação de Iluminação (Toon Shading)
    float lambert = max(0.0, dot(n,l));
    
    // Discretiza o lambert para criar o efeito Toon/Cel Shading
    float steps = 4.0;
    lambert = floor(lambert * steps) / steps;
    
    // Blinn-Phong para o especular
    vec3 h = normalize(v + l);
    float specular = pow(max(0.0, dot(n, h)), q);
    
    // Corte duro para o brilho (highlights cartunescos)
    if (specular > 0.5) {
        specular = 1.0;
    } else {
        specular = 0.0;
    }

    float distance = length(g_PosLuz - p.xyz);

    float constant = 0.3;
    float linear = 0.0001;
    float quadratic = 0.005;

    float atenuacao = 1.0 / ( constant + linear * distance + quadratic * distance * distance);

        color.rgb = ( Kd0 * (lambert + 0.2) + Ks * specular) * atenuacao;

    // NOTE: Se você quiser fazer o rendering de objetos transparentes, é
    // necessário:
    // 1) Habilitar a operação de "blending" de OpenGL logo antes de realizar o
    //    desenho dos objetos transparentes, com os comandos abaixo no código C++:
    //      glEnable(GL_BLEND);
    //      glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    // 2) Realizar o desenho de todos objetos transparentes *após* ter desenhado
    //    todos os objetos opacos; e
    // 3) Realizar o desenho de objetos transparentes ordenados de acordo com
    //    suas distâncias para a câmera (desenhando primeiro objetos
    //    transparentes que estão mais longe da câmera).
    // Alpha default = 1 = 100% opaco = 0% transparente
    color.a = 1.0;
    if (object_id == BOLA && u_BolaInativa == 1) {
        color.a = 0.4;
    }

    // Cor final com correção gamma, considerando monitor sRGB.
    // Veja https://en.wikipedia.org/w/index.php?title=Gamma_correction&oldid=751281772#Windows.2C_Mac.2C_sRGB_and_TV.2Fvideo_standard_gammas
    color.rgb = pow(color.rgb, vec3(1.0,1.0,1.0)/2.2);
} 

