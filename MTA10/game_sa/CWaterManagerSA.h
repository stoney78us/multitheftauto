/*****************************************************************************
*
*  PROJECT:		Multi Theft Auto v1.0
*  LICENSE:		See LICENSE in the top level directory
*  FILE:		game_sa/CWaterManagerSA.h
*  PURPOSE:		Control the lakes and seas
*  DEVELOPERS:	arc_
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CWATERMANAGERSA_H
#define __CWATERMANAGERSA_H

#define FUNC_ReadWaterConfiguration        0x6EAE80         // ()
#define FUNC_CreateWaterVertex             0x6E5A40         // (word x, word y, float z, float unknown, float height, word flow)
#define FUNC_CreateWaterQuad               0x6E7EF0         // (word x1, word y1, float z1, float unknown1, float height1, word flow1, word x2, ..., word x3, ..., word x4, ..., word flags)
#define FUNC_CreateWaterTriangle           0x6E7D40         // (word x1, word y1, float z1, float unknown1, float height1, word flow1, word x2, ..., word x3, ..., word flags)
#define FUNC_BuildWaterIndex               0x6E7B30         // ()
#define FUNC_AddWaterPolyToZone            0x6E5750         // (zoneRow, zoneColumn, polyID, polyType)
#define FUNC_GetWaterLevel                 0x6EB690
#define FUNC_TestLineAgainstWater          0x6E61B0

#define VAR_NumWaterVertices               0xC2288C
#define VAR_NumWaterQuads                  0xC22888
#define VAR_NumWaterTriangles              0xC22884
#define VAR_NumUsedWaterZonePolySlots      0xC215F0

#define NUM_DefWaterVertices               0x3FD
#define NUM_DefWaterQuads                  0x12D
#define NUM_DefWaterTriangles              0x6
#define NUM_WaterZones                     (12 * 12)

#define ARRAY_WaterVertices                0xC22910
#define ARRAY_WaterQuads                   0xC21C90
#define ARRAY_WaterTriangles               0xC22854
#define ARRAY_WaterZones                   0xC21B70
#define ARRAY_WaterZonePolys               0xC215F8

// -------------------------------
// SA interfaces

class CWaterVertexSAInterface
{
public:
    short m_sX;
    short m_sY;
    float m_fZ;
    float m_fUnknown;
    float m_fHeight;
    char m_cFlowX;
    char m_cFlowY;
    WORD m_wPadding;   
};

class CWaterPolySAInterface
{
public:
};

class CWaterQuadSAInterface : public CWaterPolySAInterface
{
public:
    WORD m_wVertexIDs[4];
    WORD m_wFlags;
};

class CWaterTriangleSAInterface : public CWaterPolySAInterface
{
public:
    WORD m_wVertexIDs[3];
    WORD m_wFlags;
};

class CWaterPolyEntrySAInterface
{
public:
    WORD m_wValue;
};

// -------------------------------
// Interface wrappers

class CWaterVertexSA : public CWaterVertex
{
public:
                                     CWaterVertexSA    () { m_pInterface = NULL; }
                                     CWaterVertexSA    ( CWaterVertexSAInterface* pInterface ) { m_pInterface = pInterface; }

    CWaterVertexSAInterface*         GetInterface      () { return m_pInterface; }
    void                             SetInterface      ( CWaterVertexSAInterface* pInterface ) { m_pInterface = pInterface; }

    void                             GetPosition       ( CVector& vec );
    void                             SetPosition       ( CVector& vec, void* pChangeSource = NULL );

protected:
    CWaterVertexSAInterface*         m_pInterface;
};

class CWaterPolySA : public CWaterPoly
{
public:
    CWaterPolySAInterface*           GetInterface      () { return m_pInterface; }
    virtual void                     SetInterface      ( CWaterPolySAInterface* pInterface ) = 0;

    virtual EWaterPolyType           GetType           () = 0;
    virtual int                      GetNumVertices    () = 0;
    WORD                             GetID             () { return m_wID; }
    CWaterVertex*                    GetVertex         ( int index );
    bool                             ContainsPoint     ( float fX, float fY );

protected:
    CWaterPolySAInterface*           m_pInterface;
    WORD                             m_wID;
};

class CWaterQuadSA : public CWaterPolySA
{
public:
                                     CWaterQuadSA      () { m_pInterface = NULL; m_wID = ~0; }
                                     CWaterQuadSA      ( CWaterPolySAInterface* pInterface ) { m_pInterface = pInterface; }

    CWaterQuadSAInterface*           GetInterface      () { return (CWaterQuadSAInterface *)m_pInterface; }
    void                             SetInterface      ( CWaterPolySAInterface* pInterface );

    EWaterPolyType                   GetType           () { return WATER_POLY_QUAD; }
    int                              GetNumVertices    () { return 4; }
};

class CWaterTriangleSA : public CWaterPolySA
{
public:
                                     CWaterTriangleSA  () { m_pInterface = NULL; m_wID = ~0; }
                                     CWaterTriangleSA  ( CWaterPolySAInterface* pInterface ) { m_pInterface = pInterface; }

    CWaterTriangleSAInterface*       GetInterface      () { return (CWaterTriangleSAInterface *)m_pInterface; }
    void                             SetInterface      ( CWaterPolySAInterface* pInterface );

    EWaterPolyType                   GetType           () { return WATER_POLY_TRIANGLE; }
    int                              GetNumVertices    () { return 3; }
};

class CWaterZoneSA
{
public:
                                     CWaterZoneSA      () { m_pInterface = NULL; }
                                     CWaterZoneSA      ( CWaterPolyEntrySAInterface* pInterface ) { m_pInterface = pInterface; }

    CWaterPolyEntrySAInterface*      GetInterface      () { return m_pInterface; }
    void                             SetInterface      ( CWaterPolyEntrySAInterface* pInterface ) { m_pInterface = pInterface; }

    class iterator
    {
    public:
                                     iterator          ();
                                     iterator          ( CWaterZoneSA* pZone );
                                     iterator          ( iterator& other );
        iterator&                    operator=         ( iterator& other );
        void                         operator++        ();
        void                         operator--        ();
        bool                         operator==        ( iterator& other );
        bool                         operator!=        ( iterator& other );
        CWaterPolySA*                operator*         ();
        
    private:
        CWaterPolyEntrySAInterface*  m_pFirst;
        CWaterPolyEntrySAInterface*  m_pCurrent;
        bool                         m_bSinglePoly;
    };

    iterator                         begin             ();
    int                              GetID             ();

private:
    CWaterPolyEntrySAInterface*      m_pInterface;
};

// -------------------------------
// Change trackkeepers

class CWaterChange
{
public:
    virtual void                     Undo              ( void* pChangedObject ) = 0;
};

class CWaterChangeVertexMove : public CWaterChange
{
public:
                                     CWaterChangeVertexMove ( CWaterVertex* pVertex );
    void                             Undo              ( void* pChangedObject );
private:
    CVector                          m_vecOriginalPosition;
};

// -------------------------------
// Manager

class CWaterManagerSA : public CWaterManager
{
public:
                                     CWaterManagerSA   ();
                                     ~CWaterManagerSA  ();

    CWaterPoly*                      GetPolyAtPoint    ( CVector& vecPosition );
    CWaterZoneSA*                    GetZoneContaining ( float fX, float fY );

    bool                             GetWaterLevel     ( CVector& vecPosition, float* pfLevel, bool bCheckWaves, CVector* pvecUnknown );
    bool                             SetWaterLevel     ( CVector& vecPosition, float fLevel, void* pChangeSource = NULL );
    bool                             TestLineAgainstWater ( CVector& vecStart, CVector& vecEnd, CVector* vecCollision );

    void                             AddChange         ( void* pChangeSource, void* pChangedObject, CWaterChange* pChange );
    void                             UndoChanges       ( void* pChangeSource = NULL );
    void                             Reset             ();

private:
    std::vector < CWaterVertexSA >   m_Vertices;
    std::vector < CWaterQuadSA >     m_Quads;
    std::vector < CWaterTriangleSA > m_Triangles;
    std::vector < CWaterZoneSA >     m_Zones;

    std::map < void*, std::map < void*, CWaterChange* > > m_Changes;

    friend class CWaterVertexSA;
    friend class CWaterPolySA;
    friend class CWaterZoneSA;
    friend class CWaterZoneSA::iterator;
};

#endif