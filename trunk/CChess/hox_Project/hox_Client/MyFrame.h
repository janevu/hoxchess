/***************************************************************************
 *  Copyright 2007 Huy Phan  <huyphan@playxiangqi.com>                     *
 *                                                                         * 
 *  This file is part of HOXChess.                                         *
 *                                                                         *
 *  HOXChess is free software: you can redistribute it and/or modify       *
 *  it under the terms of the GNU General Public License as published by   *
 *  the Free Software Foundation, either version 3 of the License, or      *
 *  (at your option) any later version.                                    *
 *                                                                         *
 *  HOXChess is distributed in the hope that it will be useful,            *
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of         *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the          *
 *  GNU General Public License for more details.                           *
 *                                                                         *
 *  You should have received a copy of the GNU General Public License      *
 *  along with HOXChess.  If not, see <http://www.gnu.org/licenses/>.      *
 ***************************************************************************/

/////////////////////////////////////////////////////////////////////////////
// Name:            MyFrame.h
// Created:         10/02/2007
//
// Description:     The main Frame of the App.
/////////////////////////////////////////////////////////////////////////////

#ifndef __INCLUDED_MY_FRAME_H_
#define __INCLUDED_MY_FRAME_H_

#include <wx/wx.h>
#include <wx/laywin.h>   // wxSashLayoutWindow
#include <wx/treectrl.h>
#include <wx/progdlg.h>
#include <list>
#include "hoxEnums.h"
#include "hoxTypes.h"

/* Forward declarations */
class hoxTable;
class MyChild;
class hoxLocalPlayer;
class hoxSite;

// menu items ids
enum
{
    MDI_QUIT = wxID_EXIT,
    MDI_ABOUT = wxID_ABOUT,

    MDI_NEW_TABLE = wxID_HIGHEST,
    MDI_CLOSE_TABLE,

    MDI_OPEN_SERVER,    // Open server
    MDI_CLOSE_SERVER,   // Close server

    MDI_CONNECT_SERVER, // Connect to server
    MDI_DISCONNECT_SERVER, // Disconnect from server
    MDI_LIST_TABLES,     // Get list of tables.

    MDI_CONNECT_HTTP_SERVER,
    MDI_SHOW_SERVERS_WINDOW,
    MDI_SHOW_LOG_WINDOW,

    MDI_TOGGLE,   // toggle view
    MDI_CHILD_QUIT,

    // Windows' IDs.
    ID_WINDOW_SITES,
    ID_TREE_SITES,
    ID_WINDOW_LOG
};

/* 
 * Log events
 */
DECLARE_EVENT_TYPE(hoxEVT_FRAME_LOG_MSG, wxID_ANY)

/**
 * The main (MDI) frame acting as the App's main GUI.
 * It manages a list of MDI child frames.
 *
 * @see MyChild
 */
class MyFrame : public wxMDIParentFrame
{
    typedef std::list<MyChild*> MyChildList;

public:
    MyFrame();  // Dummy default constructor required for RTTI info.
    MyFrame( wxWindow*          parent, 
             const wxWindowID   id, 
             const wxString&    title,
             const wxPoint&     pos, 
             const wxSize&      size, 
             const long         style );

    ~MyFrame();

    // -----
    void SetupMenu();
    void SetupStatusBar();

    void InitToolBar(wxToolBar* toolBar);

    void OnSize(wxSizeEvent& event);
    void OnServersSashDrag(wxSashEvent& event);
    void OnLogSashDrag(wxSashEvent& event);
    void OnAbout(wxCommandEvent& event);
    void OnNewTable(wxCommandEvent& event);
    void OnCloseTable(wxCommandEvent& event);
    void OnUpdateCloseTable(wxUpdateUIEvent& event);

    void OnOpenServer(wxCommandEvent& event);
    void OnCloseServer(wxCommandEvent& event);
    void OnConnectServer(wxCommandEvent& event);
    void OnDisconnectServer(wxCommandEvent& event);
    void OnListTables(wxCommandEvent& event);

    void OnConnectHTTPServer(wxCommandEvent& event);

    void OnShowServersWindow(wxCommandEvent& event);
    void OnUpdateServersWindow(wxUpdateUIEvent& event);

    void OnShowLogWindow(wxCommandEvent& event);
    void OnUpdateLogWindow(wxUpdateUIEvent& event);

    void OnQuit(wxCommandEvent& event);
    void OnClose(wxCloseEvent& event);

    /**
     * Invoke by the Child window when it is being closed.
     * NOTE: This *special* API is used by the Child to ask the Parent
     *       for the permission to close.
     *       !!! BE CAREFUL with recursive calls. !!!
     */
    bool OnChildClose(MyChild* child, hoxTable* table);

    void OnFrameLogMsgEvent( wxCommandEvent &event );
    void OnContextMenu( wxContextMenuEvent& event );

    void DoJoinExistingTable(const hoxNetworkTableInfo& tableInfo, hoxLocalPlayer* localPlayer);
    void DoJoinNewTable(const wxString& tableId, hoxLocalPlayer* localPlayer);

    void Handle_PlayerResponse( hoxResponse*    pResponse,
                                hoxLocalPlayer* localPlayer );

    /**
     * Create a GUI Frame that can be used as a frame for a new Table.
     *
     * @param requestTableId If this Id is specified (NOT empty), then this
     *                       function will use it. Otherwise, a new table-Id
     *                       is created and is returned to the caller.
     */
    MyChild* CreateFrameForTable( wxString& requestTableId );

    class SiteTreeItemData : public wxTreeItemData
    {
    public:
        SiteTreeItemData(hoxSite* site = NULL) : m_site(site) {}
        ~SiteTreeItemData() {}

        hoxSite* GetSite() const { return m_site; }

    private:
        hoxSite*  m_site;
    };

    class TableTreeItemData : public wxTreeItemData
    {
    public:
        TableTreeItemData(hoxTable* table = NULL) : m_table(table) {}
        ~TableTreeItemData() {}

        hoxTable* GetTable() const { return m_table; }

    private:
        hoxTable*  m_table;
    };
    void UpdateSiteTreeUI();

private:
    void _OnResponse_Leave( const wxString& responseStr );
    void _OnResponse_Connect( const wxString& responseStr, hoxLocalPlayer* localPlayer );
    void _OnResponse_List( const wxString& responseStr, hoxLocalPlayer* localPlayer );
    void _OnResponse_Join( const wxString& responseStr, hoxLocalPlayer* localPlayer );
    void _OnResponse_New( const wxString& responseStr, hoxLocalPlayer* localPlayer );

    hoxResult _GetServerAddressFromUser( const wxString&         message,
                                         const wxString&         caption,
                                         const hoxServerAddress& defaultAddress,
                                         hoxServerAddress&       serverAddress );

    hoxTable* _CreateNewTable( const wxString& tableId );

    /**
     * Close all children (child-frame) of a specified Site.
     */
    void     _CloseChildrenOfSite(hoxSite* site);

    hoxSite* _GetSelectedSite(hoxTable*& selectedTable) const;

public: /* Static API */
    static wxMenuBar* Create_Menu_Bar( bool hasTable = false );

private:
    // Servers.
    wxSashLayoutWindow* m_sitesWindow;
    wxTreeCtrl*         m_sitesTree;

    // Logging.  
    wxSashLayoutWindow* m_logWindow; // To contain the log-text below.
    wxTextCtrl*         m_logText;   // Log window for debugging purpose.

    // the progress dialog which we show while worker thread is running
    wxProgressDialog*   m_dlgProgress;

    int                 m_nChildren;   // The number of child-frames.
    MyChildList         m_children;

    DECLARE_DYNAMIC_CLASS(MyFrame)
    DECLARE_EVENT_TABLE()
};


#endif  /* __INCLUDED_MY_FRAME_H_ */